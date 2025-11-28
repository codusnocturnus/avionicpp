#pragma once

#include <array>
#include <coroutine>
#include <cstdint>
#include <expected>
#include <optional>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "mavlink/types.hpp"

namespace mavlink {

struct InputAwaiter;

/// @brief A coroutine-based Mavlink message framer.
struct Framer {
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;
    Handle handle;

    using ErrorType = std::pair<MavlinkError, std::string_view>;
    using ParseResult = std::expected<MessageView, ErrorType>;
    using YieldedValue = std::optional<ParseResult>;

    explicit Framer(Handle h) : handle(h) {}
    ~Framer() {
        if (handle)
            handle.destroy();
    }

    /// @brief Pushes a byte into the framer and retrieves any yielded result.
    /// @param[in] c The character to process.
    /// @return An optional ParseResult containing a MessageView or error if a message completed.
    [[nodiscard]] YieldedValue push_byte(std::uint8_t c);

    struct promise_type {
        std::uint8_t current_input;
        YieldedValue current_output = std::nullopt;
        bool just_yielded = false;

        Framer get_return_object() { return Framer{Handle::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }

        std::suspend_always yield_value(MessageView view) {
            current_output = ParseResult(view);
            just_yielded = true;
            return {};
        }

        std::suspend_always yield_value(ErrorType error) {
            current_output = std::unexpected(error);
            just_yielded = true;
            return {};
        }

        InputAwaiter await_transform(InputAwaiter);
    };
};

struct InputAwaiter {
    Framer::promise_type* promise;
    explicit InputAwaiter(Framer::promise_type* p = nullptr) : promise(p) {}
    bool await_ready() {
        if (promise->just_yielded) {
            promise->just_yielded = false;
            return true;
        }
        return false;
    }
    void await_suspend(std::coroutine_handle<>) {}
    std::uint8_t await_resume() { return promise->current_input; }
};

inline InputAwaiter Framer::promise_type::await_transform(InputAwaiter) {
    return InputAwaiter{this};
}

inline Framer::YieldedValue Framer::push_byte(std::uint8_t c) {
    handle.promise().current_input = c;
    handle.promise().current_output = std::nullopt;
    if (!handle.done()) {
        handle.resume();
    }
    return std::move(handle.promise().current_output);
}

/// @brief Creates a coroutine-based Mavlink parser state machine.
/// @param[in] active_buffer_ptr Pointer to a span used as a working buffer.
/// @return A Framer instance.
inline Framer create_framer(std::span<std::uint8_t>* active_buffer_ptr) {
    while (true) {
        auto view = MessageView{};
        auto magic = std::uint8_t{0};
        auto idx = size_t{0};

        // 1. Wait for STX
        do {
            magic = co_await InputAwaiter{};
        } while (magic != 0xFD && magic != 0xFE);
        (*active_buffer_ptr)[idx++] = magic;

        // 2. Read Payload Length
        auto len = co_await InputAwaiter{};
        (*active_buffer_ptr)[idx++] = len;

        // 3. Read Header Fields
        if (magic == 0xFD) {
            // Mavlink v2
            // INC_FLAGS
            (*active_buffer_ptr)[idx++] = co_await InputAwaiter{};
            // CMP_FLAGS
            (*active_buffer_ptr)[idx++] = co_await InputAwaiter{};
        }
        // common to Mavlink v1 and v2
        // SEQ
        view.seq = co_await InputAwaiter{};
        (*active_buffer_ptr)[idx++] = view.seq;
        // SYS
        view.sysid = co_await InputAwaiter{};
        (*active_buffer_ptr)[idx++] = view.sysid;
        // COMP
        view.compid = co_await InputAwaiter{};
        (*active_buffer_ptr)[idx++] = view.compid;
        // first byte of MSGID
        view.msgid = co_await InputAwaiter{} & 0xFF;
        (*active_buffer_ptr)[idx++] = view.msgid;

        if (magic == 0xFD) {
            // Mavlink v2 MSGID is 3 bytes, total, little endian
            auto id_mid = co_await InputAwaiter{};
            view.msgid |= id_mid << 8;
            (*active_buffer_ptr)[idx++] = id_mid;
            auto id_hi = co_await InputAwaiter{};
            view.msgid |= id_hi << 16;
            (*active_buffer_ptr)[idx++] = id_hi;
        }

        // Payload Starts at the current index
        view.payload = std::span<const std::uint8_t>(active_buffer_ptr->data() + idx, len);

        // 4. Read Payload
        for ([[maybe_unused]] auto i : std::ranges::iota_view{size_t{0}, static_cast<size_t>(len)}) {
            auto b = co_await InputAwaiter{};
            if (idx >= active_buffer_ptr->size()) {
                co_yield std::make_pair(MavlinkError::BufferOverrun, "Buffer overrun");
                goto next_message;
            }
            (*active_buffer_ptr)[idx++] = b;
        }

        // 5. Read CRC (2 bytes)
        {
            auto crc1 = co_await InputAwaiter{};
            (*active_buffer_ptr)[idx++] = crc1;
            auto crc2 = co_await InputAwaiter{};
            (*active_buffer_ptr)[idx++] = crc2;
        }

        // 6. Signature (v2 only, if incompatible flags & 0x01)
        // Check incompat flags (byte at index 2 for v2)
        if (magic == 0xFD) {
            auto inc_flags = (*active_buffer_ptr)[2];
            if (inc_flags & 0x01) {
                // Read 13 bytes signature
                for (auto i : std::ranges::iota_view{0, 13}) {
                    (void)i;  // Unused
                    auto b = co_await InputAwaiter{};
                    if (idx >= active_buffer_ptr->size()) {
                        co_yield std::make_pair(MavlinkError::BufferOverrun, "Buffer overrun");
                        goto next_message;
                    }
                    (*active_buffer_ptr)[idx++] = b;
                }
            }
        }

        // TODO: Validate Checksum here if we had the CRC extra info.

        co_yield view;

    next_message:;
    }
}

}  // namespace mavlink
