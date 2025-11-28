#pragma once

#include <chrono>
#include <expected>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>

#include "settings.hpp"

namespace uart {

template <auto ReadCallbackType>
class StubUART {
   public:
    using native_handle_type = std::nullptr_t;

    // StubUART(std::string_view devicename, ReadCallbackType readcallback)
    //     : devicename_{devicename}, readcallback_{readcallback_}, writecallback_{nullptr} {}
    StubUART(std::string_view devicename) : StubUART{devicename_, nullptr} {}

    /// @return nullptr
    [[nodiscard]] auto native_handle() const noexcept -> native_handle_type { return nullptr; }

    [[nodiscard]] auto devicename() const noexcept -> std::string_view { return devicename_; }
    [[nodiscard]] auto baudrate() const noexcept -> BaudRate { return baudrate_; }
    [[nodiscard]] auto set_baudrate(BaudRate baudrate) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        baudrate_ = baudrate;
        return true;
    }
    [[nodiscard]] auto charactersize() const noexcept -> CharacterSize { return charactersize_; }
    [[nodiscard]] auto set_charactersize(CharacterSize charactersize) noexcept
        -> std::expected<bool, std::pair<int, std::string>> {
        charactersize_ = charactersize;
        return true;
    }
    [[nodiscard]] auto parity() const noexcept -> Parity { return parity_; }
    [[nodiscard]] auto set_parity(Parity parity) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        parity_ = parity;
        return true;
    }
    [[nodiscard]] auto stopbits() const noexcept -> StopBits { return stopbits_; }
    [[nodiscard]] auto set_stopbits(StopBits stopbits) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        stopbits_ = stopbits;
        return true;
    }
    [[nodiscard]] auto timeout() const noexcept -> std::chrono::milliseconds { return timeout_; }
    [[nodiscard]] auto set_timeout(std::chrono::milliseconds timeout) noexcept
        -> std::expected<bool, std::pair<int, std::string>> {
        timeout_ = timeout;
        return true;
    }

    /// @return true
    [[nodiscard]] auto open() noexcept -> std::expected<bool, std::pair<int, std::string>> { return isopen_ = true; }
    void close() noexcept { isopen_ = false; }
    [[nodiscard]] auto is_open() const noexcept -> bool { return isopen_; }

    /// @param buffer [out] range type, no changes will be made
    /// @param readsize minimum number of bytes to return (max will be taken from buffer size)
    /// @return zero
    [[nodiscard]] auto read([[maybe_unused]] std::ranges::sized_range auto& buffer,
                            [[maybe_unused]] std::size_t readsize) noexcept
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return 0;
    }

    /// @param buffer range type
    /// @return size of buffer
    [[nodiscard]] auto write([[maybe_unused]] const std::ranges::sized_range auto& buffer) noexcept
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return std::ranges::size(buffer);
    }

   private:
    const std::string devicename_;
    BaudRate baudrate_{BaudRate::b9600};
    CharacterSize charactersize_{CharacterSize::cs8};
    Parity parity_{Parity::none};
    StopBits stopbits_{StopBits::sb1};
    std::chrono::milliseconds timeout_{0};
    bool isopen_{false};

    // std::function<ReadCallbackType> readcallback_;
    // std::function<std::expected<std::size_t, std::pair<int, std::string>>()> writecallback_;
};

}  // namespace uart
