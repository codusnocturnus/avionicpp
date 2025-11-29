#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <expected>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>

#define NOMINMAX
#include <windows.h>

#include "settings.hpp"

namespace uart {

class Win32UART {
   public:
    using native_handle_type = HANDLE;

    Win32UART(std::string_view devicename) : devicename_{devicename} {}
    ~Win32UART() { close(); }
    Win32UART(const Win32UART&) = delete;
    Win32UART(Win32UART&&) = delete;
    auto operator=(const Win32UART&) = delete;
    auto operator=(Win32UART&&) = delete;

    /// @return OS-native handle to the underlying UART object
    [[nodiscard]] auto native_handle() const noexcept -> native_handle_type { return uarthandle_; }

    [[nodiscard]] auto devicename() const noexcept -> std::string_view { return devicename_; }
    [[nodiscard]] auto baudrate() const noexcept -> BaudRate;
    auto set_baudrate(BaudRate baud) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto charactersize() const noexcept -> CharacterSize;
    auto set_charactersize(CharacterSize charsize) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto parity() const noexcept -> Parity;
    auto set_parity(Parity parity) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto stopbits() const noexcept -> StopBits;
    auto set_stopbits(StopBits stopbits) -> std::expected<bool, std::pair<int, std::string>>;
    [[nodiscard]] auto timeout() const noexcept -> std::chrono::milliseconds;
    auto set_timeout(std::chrono::milliseconds timeout_ms) -> std::expected<bool, std::pair<int, std::string>>;

    /// @brief Will close the UART if already open
    /// @return true if UART is successfully opened and configured,
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
        auto bytesread = DWORD{0};
        auto count = std::min(std::ranges::size(buffer), readsize);
        if (count > std::numeric_limits<DWORD>::max()) {
            count = std::numeric_limits<DWORD>::max();
        }
        if (ReadFile(uarthandle_, std::ranges::data(buffer), static_cast<DWORD>(count), &bytesread, NULL) == FALSE) {
            auto err = GetLastError();
            return std::unexpected<std::pair<int, std::string>>{
                std::make_pair(static_cast<int>(err), win32error2string(err))};
        }
        return static_cast<std::size_t>(bytesread);
    }

    /// @param buffer range type with data to be written
    /// @return number of bytes written if successful,
    ///         error code and string via std::unexpected if write failed
    [[nodiscard]] auto write(const std::ranges::sized_range auto& buffer)
        -> std::expected<std::size_t, std::pair<int, std::string>> {
        auto byteswritten = DWORD{0};
        auto count = std::ranges::size(buffer);
        if (count > std::numeric_limits<DWORD>::max()) {
            count = std::numeric_limits<DWORD>::max();
        }
        if (not WriteFile(uarthandle_, std::ranges::cdata(buffer), static_cast<DWORD>(count), &byteswritten, NULL)) {
            auto err = GetLastError();
            return std::unexpected<std::pair<int, std::string>>{
                std::make_pair(static_cast<int>(err), win32error2string(err))};
        }
        return static_cast<std::size_t>(byteswritten);
    }

   private:
    HANDLE uarthandle_{NULL};
    const std::string devicename_;
    DWORD baudrate_{CBR_9600};
    WORD charactersize_{DATABITS_8};
    BYTE parity_{NOPARITY};
    BYTE stopbits_{ONESTOPBIT};
    std::chrono::milliseconds timeout_{0};
    bool isopen_{false};

    DCB currentdcb_{0};

    const static std::unordered_map<BaudRate, DWORD> enum2baud_;
    const static std::unordered_map<DWORD, BaudRate> baud2enum_;
    const static std::unordered_map<CharacterSize, WORD> enum2charsize_;
    const static std::unordered_map<WORD, CharacterSize> charsize2enum_;
    const static std::unordered_map<Parity, BYTE> enum2parity_;
    const static std::unordered_map<BYTE, Parity> parity2enum_;

    /// @return true if the uart is open and changes are made,
    ///         false if the uart isn't open,
    ///         error code and string via std::unexpected if configuration changes failed
    auto configure() -> std::expected<bool, std::pair<int, std::string>>;

    /// @brief Format an error string, given an error code.
    /// @param code value from GetLastError()
    /// @return text description of the error
    static auto win32error2string(DWORD code) -> std::string;
};

}  // namespace uart
