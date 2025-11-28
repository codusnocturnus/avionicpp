#pragma once

#include <chrono>
#include <cstdint>
#include <expected>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>

#include "settings.hpp"

namespace uart {

template <typename UARTType>
/// @note This class is not thread-safe. External synchronization is required if instances are shared across threads.
class UART {
   public:
    using native_handle_type = UARTType::native_handle_type;

    /// @brief default constructor creates a uart object using the template parameter type
    explicit UART(std::string_view devicename) : uart_{std::make_shared<UARTType>(devicename)} {}
    /// @brief dependency injection constructor uses the given uart as-is - mostly for testing
    explicit UART(std::shared_ptr<UARTType> uart) noexcept : uart_{uart} {}
    virtual ~UART() = default;
    UART(const UART&) = delete;
    UART(UART&&) = delete;
    auto operator=(const UART&) = delete;
    auto operator=(UART&&) = delete;

    /// @return OS-native handle to the underlying uart object
    auto native_handle() const noexcept -> UARTType::native_handle_type { return uart_->native_handle(); }

    [[nodiscard]] auto devicename() const noexcept -> std::string_view { return uart_->devicename(); }
    [[nodiscard]] auto baudrate() const noexcept -> BaudRate { return uart_->baudrate(); }
    auto set_baudrate(BaudRate baud) -> std::expected<bool, std::pair<int, std::string>> {
        return uart_->set_baudrate(baud);
    }
    [[nodiscard]] auto charactersize() const noexcept -> CharacterSize { return uart_->charactersize(); }
    auto set_charactersize(CharacterSize charsize) -> std::expected<bool, std::pair<int, std::string>> {
        return uart_->set_charactersize(charsize);
    }
    [[nodiscard]] auto parity() const noexcept -> Parity { return uart_->parity(); }
    auto set_parity(Parity parity) -> std::expected<bool, std::pair<int, std::string>> {
        return uart_->set_parity(parity);
    }
    [[nodiscard]] auto stopbits() const noexcept -> StopBits { return uart_->stopbits(); }
    auto set_stopbits(StopBits stopbits) -> std::expected<bool, std::pair<int, std::string>> {
        return uart_->set_stopbits(stopbits);
    }
    [[nodiscard]] auto timeout() const noexcept -> std::chrono::milliseconds { return uart_->timeout(); }
    auto set_timeout(std::chrono::milliseconds timeout_ms) -> std::expected<bool, std::pair<int, std::string>> {
        return uart_->set_timeout(timeout_ms);
    }

    /// @brief Will close the uart if already open
    /// @return true if uart is successfully opened and configured,
    ///         error code and string via std::unexpected if opening or configuration failed
    auto open() -> std::expected<bool, std::pair<int, std::string>> { return uart_->open(); }
    [[nodiscard]] auto is_open() const noexcept { return uart_->is_open(); }
    void close() { uart_->close(); }

    /// @param buffer [out] range type into which read data will be stored
    /// @param readsize[in] minimum number of bytes to return (max will be taken from buffer size)
    /// @return number of bytes read into buffer if successful,
    ///         error code and string via std::unexpected if write failed
    [[nodiscard]] auto read(std::ranges::range auto& buffer, std::size_t readsize)
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return uart_->read(buffer, readsize);
    }

    /// @param[in] buffer range type with data to be written
    /// @return number of bytes written if successful,
    ///         error code and string via std::unexpected if write failed
    [[nodiscard]] auto write(const std::ranges::range auto& buffer)
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        return uart_->write(buffer);
    }

   private:
    std::shared_ptr<UARTType> uart_;
};

}  // namespace uart
