#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstring>
#include <expected>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "settings.hpp"

namespace uart {

class PosixUART {
   public:
    using native_handle_type = int;

    PosixUART(std::string_view devicename) noexcept : devicename_(devicename) {}
    ~PosixUART() { close(); }
    PosixUART(const PosixUART&) = delete;
    PosixUART(PosixUART&&) = delete;
    auto operator=(const PosixUART&) = delete;
    auto operator=(PosixUART&&) = delete;

    /// @return OS-native handle to the underlying uart object
    [[nodiscard]] auto native_handle() const noexcept -> native_handle_type { return uarthandle_; }

    [[nodiscard]] auto devicename() const noexcept -> std::string_view { return devicename_; }
    [[nodiscard]] auto baudrate() const noexcept -> BaudRate;
    auto set_baudrate(BaudRate baud) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto charactersize() const noexcept -> CharacterSize;
    auto set_charactersize(CharacterSize charsize) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto parity() const noexcept -> Parity;
    auto set_parity(Parity parity) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto stopbits() const noexcept -> StopBits {
        return (stopbits_ == 0U) ? StopBits::sb1 : StopBits::sb2;
    }
    auto set_stopbits(StopBits stopbits) -> std::expected<bool, std::pair<int, std::string>> {
        stopbits_ = (stopbits == StopBits::sb1) ? 0U : CSTOPB;
        return configure();
    }
    [[nodiscard]] auto timeout() const noexcept -> std::chrono::milliseconds;
    auto set_timeout(std::chrono::milliseconds timeout_ms) -> std::expected<bool, std::pair<int, std::string>>;

    /// @brief Will close the uart if already open
    /// @return true if uart is successfully opened and configured,
    ///         error code and string via std::unexpected if opening or configuration failed
    [[nodiscard]] auto open() -> std::expected<bool, std::pair<int, std::string>>;

    [[nodiscard]] auto is_open() const noexcept -> bool { return isopen_; }

    void close();

    /// @param buffer [out] range type into which read data will be stored
    /// @param readsize minimum number of bytes to return (max will be taken from buffer size)
    /// @return number of bytes read into buffer if successful,
    ///         error code and string via std::unexpected if write failed
    [[nodiscard]] auto read(std::ranges::sized_range auto& buffer, std::size_t readsize)
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        auto count = std::min(std::ranges::size(buffer), readsize);
        if (count > SSIZE_MAX) {
            count = SSIZE_MAX;
        }
        auto result = ::read(uarthandle_, std::ranges::data(buffer), count);
        if (result < 0) {
            return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
        }
        return result;
    }

    /// @param buffer range type with data to be written
    /// @return number of bytes written if successful,
    ///         error code and string via std::unexpected if write failed
    [[nodiscard]] auto write(const std::ranges::sized_range auto& buffer)
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        auto count = std::ranges::size(buffer);
        if (count > SSIZE_MAX) {
            count = SSIZE_MAX;
        }
        auto result = ::write(uarthandle_, std::ranges::cdata(buffer), count);
        if (result < 0) {
            return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
        }
        return result;
    }

   private:
    int uarthandle_{-1};
    const std::string devicename_;
    speed_t baudrate_{B9600};
    std::uint8_t charactersize_{CS8};
    tcflag_t parity_{0};
    std::uint8_t stopbits_{0U};
    cc_t timeout_deciseconds_{0};
    bool isopen_{false};

    termios currenttio_{0};

    const static std::unordered_map<BaudRate, std::pair<speed_t, int>> enum2baud_;
    const static std::unordered_map<speed_t, BaudRate> baud2enum_;
    const static std::unordered_map<CharacterSize, std::uint8_t> enum2charsize_;
    const static std::unordered_map<std::uint8_t, CharacterSize> charsize2enum_;
    const static std::unordered_map<Parity, tcflag_t> enum2parity_;
    const static std::unordered_map<tcflag_t, Parity> parity2enum_;

    /// @return true if the uart is open and changes are made,
    ///         false if the uart isn't open,
    ///         error code and string via std::unexpected if configuration changes failed
    auto configure() -> std::expected<bool, std::pair<int, std::string>>;
};

}  // namespace uart
