#include <array>
#include <chrono>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>

#include <fakeit.hpp>

#include "uart/settings.hpp"

class FakeUART {
   public:
    using native_handle_type = std::nullptr_t;

    FakeUART([[maybe_unused]] std::string_view devicename) {};
    virtual ~FakeUART() {}

    [[nodiscard]] virtual auto native_handle() const noexcept -> native_handle_type { return nullptr; }

    [[nodiscard]] virtual auto devicename() const noexcept -> std::string_view { return ""; }
    [[nodiscard]] virtual auto baudrate() const noexcept -> uart::BaudRate { return uart::BaudRate::b9600; }
    virtual auto set_baudrate(uart::BaudRate baud) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        return true;
    }
    [[nodiscard]] virtual auto charactersize() const noexcept -> uart::CharacterSize {
        return uart::CharacterSize::cs8;
    }
    virtual auto set_charactersize(uart::CharacterSize charsize) noexcept
        -> std::expected<bool, std::pair<int, std::string>> {
        return true;
    }
    [[nodiscard]] virtual auto parity() const noexcept -> uart::Parity { return uart::Parity::none; }
    virtual auto set_parity(uart::Parity parity) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        return true;
    }
    [[nodiscard]] virtual auto stopbits() const noexcept -> uart::StopBits { return uart::StopBits::sb1; }
    virtual auto set_stopbits(uart::StopBits stopbits) noexcept -> std::expected<bool, std::pair<int, std::string>> {
        return true;
    }
    [[nodiscard]] virtual auto timeout() const noexcept -> std::chrono::milliseconds {
        return std::chrono::milliseconds(0);
    }
    virtual auto set_timeout(std::chrono::milliseconds timeout_ms) noexcept
        -> std::expected<bool, std::pair<int, std::string>> {
        return true;
    }

    [[nodiscard]] virtual auto open() noexcept -> std::expected<bool, std::pair<int, std::string>> { return false; }
    virtual void close() noexcept {}
    [[nodiscard]] virtual auto is_open() const noexcept -> bool { return false; }

    // NOTE: read and write use concrete types for testing, because template functions can't be virtual
    //       and functions need to be virtual to be mocked

    [[nodiscard]] virtual auto read([[maybe_unused]] std::array<std::uint8_t, 1024>& buffer,
                                    [[maybe_unused]] std::size_t readsize) noexcept
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return 0;
    }
    // passing by value as fakeit doesn't handle ref parameters properly for call verification
    [[nodiscard]] virtual auto write(const std::span<std::uint8_t> buffer) noexcept
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return false;
    }
};

using MockUART = fakeit::Mock<FakeUART>;
