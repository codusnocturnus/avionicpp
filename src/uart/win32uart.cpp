#include "win32uart.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>

#include <errhandlingapi.h>
#include <handleapi.h>
#define NOMINMAX
#include <windows.h>

namespace uart {

const std::unordered_map<BaudRate, DWORD> Win32UART::enum2baud_ = {
    {BaudRate::b110, CBR_110},       {BaudRate::b300, CBR_300},       {BaudRate::b600, CBR_600},
    {BaudRate::b1200, CBR_1200},     {BaudRate::b2400, CBR_2400},     {BaudRate::b4800, CBR_4800},
    {BaudRate::b9600, CBR_9600},     {BaudRate::b14400, CBR_14400},   {BaudRate::b19200, CBR_19200},
    {BaudRate::b38400, CBR_38400},   {BaudRate::b56000, CBR_56000},   {BaudRate::b57600, CBR_57600},
    {BaudRate::b115200, CBR_115200}, {BaudRate::b128000, CBR_128000}, {BaudRate::b256000, CBR_256000}};

const std::unordered_map<DWORD, BaudRate> Win32UART::baud2enum_ = {
    {CBR_110, BaudRate::b110},       {CBR_300, BaudRate::b300},       {CBR_600, BaudRate::b600},
    {CBR_1200, BaudRate::b1200},     {CBR_2400, BaudRate::b2400},     {CBR_4800, BaudRate::b4800},
    {CBR_9600, BaudRate::b9600},     {CBR_14400, BaudRate::b14400},   {CBR_19200, BaudRate::b19200},
    {CBR_38400, BaudRate::b38400},   {CBR_56000, BaudRate::b56000},   {CBR_57600, BaudRate::b57600},
    {CBR_115200, BaudRate::b115200}, {CBR_128000, BaudRate::b128000}, {CBR_256000, BaudRate::b256000}};

const std::unordered_map<CharacterSize, WORD> Win32UART::enum2charsize_ = {{CharacterSize::cs5, DATABITS_5},
                                                                           {CharacterSize::cs6, DATABITS_6},
                                                                           {CharacterSize::cs7, DATABITS_7},
                                                                           {CharacterSize::cs8, DATABITS_8},
                                                                           {CharacterSize::cs16, DATABITS_16}};

const std::unordered_map<WORD, CharacterSize> Win32UART::charsize2enum_ = {{DATABITS_5, CharacterSize::cs5},
                                                                           {DATABITS_6, CharacterSize::cs6},
                                                                           {DATABITS_7, CharacterSize::cs7},
                                                                           {DATABITS_8, CharacterSize::cs8},
                                                                           {DATABITS_16, CharacterSize::cs16}};

const std::unordered_map<Parity, BYTE> Win32UART::enum2parity_ = {{Parity::none, NOPARITY},
                                                                  {Parity::even, EVENPARITY},
                                                                  {Parity::odd, ODDPARITY},
                                                                  {Parity::mark, MARKPARITY},
                                                                  {Parity::space, SPACEPARITY}};
const std::unordered_map<BYTE, Parity> Win32UART::parity2enum_ = {{NOPARITY, Parity::none},
                                                                  {EVENPARITY, Parity::even},
                                                                  {ODDPARITY, Parity::odd},
                                                                  {MARKPARITY, Parity::mark},
                                                                  {SPACEPARITY, Parity::space}};

auto Win32UART::baudrate() const noexcept -> BaudRate {
    auto baud = BaudRate::b9600;
    try {
        baud = baud2enum_.at(baudrate_);
    } catch (const std::out_of_range&) {
        baud = BaudRate::b9600;
    }
    return baud;
}
auto Win32UART::set_baudrate(BaudRate baud) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        baudrate_ = enum2baud_.at(baud);
    } catch (std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid baud rate")};
    }
    return configure();
}

auto Win32UART::charactersize() const noexcept -> CharacterSize {
    auto csz = CharacterSize::cs8;
    try {
        csz = charsize2enum_.at(charactersize_);
    } catch (const std::out_of_range&) {
        csz = CharacterSize::cs8;
    }
    return csz;
}
auto Win32UART::set_charactersize(CharacterSize charsize) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        charactersize_ = enum2charsize_.at(charsize);
    } catch (const std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid character size")};
    }
    return configure();
}

auto Win32UART::parity() const noexcept -> Parity {
    auto par = Parity::none;
    try {
        par = parity2enum_.at(parity_);
    } catch (const std::out_of_range&) {
        par = Parity::none;
    }
    return par;
}
auto Win32UART::set_parity(Parity parity) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        parity_ = enum2parity_.at(parity);
    } catch (const std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid parity")};
    }
    return configure();
}

auto Win32UART::stopbits() const noexcept -> StopBits {
    return (stopbits_ == ONESTOPBIT) ? StopBits::sb1 : StopBits::sb2;
}
auto Win32UART::set_stopbits(StopBits stopbits) -> std::expected<bool, std::pair<int, std::string>> {
    // only 3 choices, and the default is 1 stop bit
    switch (stopbits) {
        case (StopBits::sb1_5):
            stopbits_ = ONE5STOPBITS;
            break;
        case (StopBits::sb2):
            stopbits_ = TWOSTOPBITS;
            break;
        default:
            stopbits_ = ONESTOPBIT;
    }
    return configure();
}

auto Win32UART::timeout() const noexcept -> std::chrono::milliseconds {
    return timeout_;
}
auto Win32UART::set_timeout(std::chrono::milliseconds timeout_ms) -> std::expected<bool, std::pair<int, std::string>> {
    // timeout_ms represents the total read/write timeout, and other timeouts will be calculated from baud rate
    // in Windows, the baud rate macros expand to the corresponding numerical value
    if (isopen_) {
        constexpr const auto bitallowance = 1.1;  // 10% allowance on bit time
        auto bittime_ms = (1.0e-3 / CBR_110) * bitallowance;
        if (baudrate_ > 0) {
            bittime_ms = (1.0e-3 / baudrate_) * bitallowance;
        }
        // various timeouts are in units of milliseconds
        auto cto = COMMTIMEOUTS{0};
        const auto timeoutmultiplier =
            std::max<DWORD>(1, static_cast<DWORD>(bittime_ms));  // windows defines max as a macro
        cto.ReadIntervalTimeout = timeoutmultiplier;
        cto.ReadTotalTimeoutConstant = timeout_ms.count();
        cto.ReadTotalTimeoutMultiplier = timeoutmultiplier;
        cto.WriteTotalTimeoutConstant = timeout_ms.count();
        cto.WriteTotalTimeoutMultiplier = timeoutmultiplier;
        if (SetCommTimeouts(uarthandle_, &cto) == FALSE) {
            auto err = GetLastError();
            return std::unexpected<std::pair<int, std::string>>{
                std::make_pair(static_cast<int>(err), win32error2string(err))};
        }
    }
    timeout_ = timeout_ms;
    return true;
}

auto Win32UART::open() -> std::expected<bool, std::pair<int, std::string>> {
    if (isopen_) {
        close();
    }

    uarthandle_ = CreateFile(devicename_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL, NULL);
    if (uarthandle_ == INVALID_HANDLE_VALUE) {
        isopen_ = false;
        auto err = GetLastError();
        return std::unexpected<std::pair<int, std::string>>{
            std::make_pair(static_cast<int>(err), win32error2string(err))};
    }
    isopen_ = true;
    if (not set_timeout(timeout_)) {
        return false;
    }
    auto result = configure();
    isopen_ = result.has_value() and result.value();
    if (not isopen_) {
        close();
    }
    return result;
}

auto Win32UART::configure() -> std::expected<bool, std::pair<int, std::string>> {
    if (not isopen_) {
        return false;
    }
    auto newdcb = DCB{0};
    newdcb.DCBlength = sizeof(DCB);
    if (GetCommState(uarthandle_, &newdcb) == FALSE) {
        auto err = GetLastError();
        return std::unexpected<std::pair<int, std::string>>{
            std::make_pair(static_cast<int>(err), win32error2string(err))};
    }
    newdcb.BaudRate = baudrate_;
    newdcb.ByteSize = charactersize_;
    newdcb.Parity = parity_;
    newdcb.StopBits = stopbits_;
    if (SetCommState(uarthandle_, &newdcb) == FALSE) {
        auto err = GetLastError();
        return std::unexpected<std::pair<int, std::string>>{
            std::make_pair(static_cast<int>(err), win32error2string(err))};
    }
    currentdcb_ = newdcb;
    return true;
}

void Win32UART::close() {
    if (isopen_) {
        CloseHandle(uarthandle_);
    }
}

auto Win32UART::win32error2string(DWORD code) -> std::string {
    auto lpMsgBuf = LPTSTR{NULL};
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                  code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);
    return std::string{lpMsgBuf};
}

}  // namespace uart
