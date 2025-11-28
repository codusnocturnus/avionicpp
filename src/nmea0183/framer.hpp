#pragma once

#include <array>
#include <charconv>
#include <coroutine>
#include <expected>
#include <optional>
#include <span>
#include <string_view>
#include <system_error>
#include <utility>

#include "types.hpp"

namespace nmea0183 {

struct InputAwaiter;  // Forward-declare

/// @brief A coroutine-based NMEA message framer.
struct Framer {
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;
    Handle handle;

    using ErrorType = std::pair<int, std::string_view>;
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
    [[nodiscard]] YieldedValue push_byte(char c);

    // --- The nested promise ---
    struct promise_type {
        char current_input;
        YieldedValue current_output = std::nullopt;

        // This flag coordinates the post-yield resume
        bool just_yielded = false;

        Framer get_return_object() { return Framer{Handle::from_promise(*this)}; }

        // Keep suspend_never to handle the very first byte
        std::suspend_never initial_suspend() { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }

        // Success yield
        std::suspend_always yield_value(MessageView view) {
            current_output = ParseResult(view);
            just_yielded = true;  // Set the flag!
            return {};
        }

        // Error yield
        std::suspend_always yield_value(ErrorType error) {
            current_output = std::unexpected(error);
            just_yielded = true;  // Set the flag!
            return {};
        }

        InputAwaiter await_transform(InputAwaiter);
    };
};

// --- The Awaiter ---
struct InputAwaiter {
    Framer::promise_type* promise;
    explicit InputAwaiter(Framer::promise_type* p = nullptr) : promise(p) {}

    /**
     * @brief Checks if we need to suspend.
     * @return 'true' (don't suspend) if we were just yielded.
     * 'false' (do suspend) in all other cases.
     */
    bool await_ready() {
        if (promise->just_yielded) {
            promise->just_yielded = false;  // Consume the flag
            return true;                    // Do not suspend, process the byte now
        }
        return false;  // Suspend and wait for a byte
    }

    void await_suspend(std::coroutine_handle<>) {}

    char await_resume() { return promise->current_input; }
};

// --- "Glue" Method Definitions ---

inline InputAwaiter Framer::promise_type::await_transform(InputAwaiter) {
    return InputAwaiter{this};
}

inline Framer::YieldedValue Framer::push_byte(char c) {
    handle.promise().current_input = c;
    handle.promise().current_output = std::nullopt;
    if (!handle.done()) {
        handle.resume();
    }
    return std::move(handle.promise().current_output);
}

namespace {
// --- Error Definitions ---
enum class ErrorCode { BUFFER_OVERRUN = 1, PROTOCOL_VIOLATION = 2, INVALID_CHECKSUM_CHAR = 3, CHECKSUM_MISMATCH = 4 };

constexpr std::string_view MSG_OVERRUN = "Buffer overrun: payload larger than provided span";
constexpr std::string_view MSG_PROTOCOL = "Protocol violation: unexpected newline or >80 char payload";
constexpr std::string_view MSG_BAD_CRLF = "Protocol violation: missing CRLF";
constexpr std::string_view MSG_INV_CHAR = "Invalid hex character in checksum";
constexpr std::string_view MSG_MISMATCH = "Checksum mismatch";
}  // namespace

/**
 * @brief Creates a coroutine-based NMEA parser state machine.
 * @param[in] active_buffer_ptr Pointer to a span used as a working buffer.
 * @return A Framer instance.
 *
 * Yields a std::expected, containing either the payload (on success) or an
 * error code and message (on failure).
 */
inline Framer create_framer(std::span<char>* active_buffer_ptr) {
    // --- Parser State Variables ---
    auto buffer_idx = size_t{0};
    auto calculated_checksum = uint8_t{0};
    auto view = MessageView{};
    auto field_start_idx = size_t{0};

    while (true) {
        auto c = char{0};
        auto hex1 = char{};
        auto hex2 = char{};

        // STATE 1: Search for Start Delimiter
        do {
            c = co_await InputAwaiter{};
        } while (c != '$' && c != '!');

        buffer_idx = 0;
        calculated_checksum = 0;
        view = {};  // Reset the view
        field_start_idx = 0;

        // STATE 2: Read Payload and Parse Fields
        auto is_first_field = true;
        while (true) {
            c = co_await InputAwaiter{};

            if (c == '*' || c == '\n' || c == ',') {
                std::string_view current_field(active_buffer_ptr->data() + field_start_idx,
                                               buffer_idx - field_start_idx);

                if (is_first_field) {
                    if (current_field.length() >= 5) {
                        view.talker_id = current_field.substr(0, 2);
                        view.message_type = current_field.substr(2, 3);
                    }
                    is_first_field = false;
                } else {
                    if (view.field_count < MessageView::MaxFields) {
                        view.fields[view.field_count++] = current_field;
                    }
                }
                field_start_idx = buffer_idx + 1;

                if (c == '*')
                    break;
                if (c == '\n') {
                    co_yield std::make_pair((int)ErrorCode::PROTOCOL_VIOLATION, MSG_PROTOCOL);
                    goto next_message;  // Using goto to break out of nested loops and restart
                }
            }

            if (buffer_idx >= active_buffer_ptr->size()) {
                co_yield std::make_pair((int)ErrorCode::BUFFER_OVERRUN, MSG_OVERRUN);
                goto next_message;
            }

            calculated_checksum ^= static_cast<uint8_t>(c);
            (*active_buffer_ptr)[buffer_idx++] = c;
        }
    next_message:;

        // STATE 3: Read Checksum
        hex1 = co_await InputAwaiter{};
        hex2 = co_await InputAwaiter{};

        // STATE 4: Read End of Line
        c = co_await InputAwaiter{};
        if (c != '\r') {
            co_yield std::make_pair((int)ErrorCode::PROTOCOL_VIOLATION, MSG_BAD_CRLF);
            continue;
        }
        c = co_await InputAwaiter{};
        if (c != '\n') {
            co_yield std::make_pair((int)ErrorCode::PROTOCOL_VIOLATION, MSG_BAD_CRLF);
            continue;
        }

        // STATE 5: Validate Checksum
        {
            auto checksum_chars = std::array<char, 2>{hex1, hex2};
            auto received_checksum = uint8_t{0};

            auto [ptr, ec] = std::from_chars(checksum_chars.data(), checksum_chars.data() + 2, received_checksum, 16);

            if (ec != std::errc() || ptr != checksum_chars.data() + 2) {
                co_yield std::make_pair((int)ErrorCode::INVALID_CHECKSUM_CHAR, MSG_INV_CHAR);
                continue;
            }

            if (received_checksum != calculated_checksum) {
                co_yield std::make_pair((int)ErrorCode::CHECKSUM_MISMATCH, MSG_MISMATCH);
                continue;
            }

            // SUCCESS: Yield the parsed view
            co_yield view;
        }
    }
}

}  // namespace nmea0183
