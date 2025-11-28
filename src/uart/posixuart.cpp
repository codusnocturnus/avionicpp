#include "posixuart.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <unordered_map>

#include <termios.h>
#include <unistd.h>

#include "settings.hpp"

namespace uart {

const std::unordered_map<BaudRate, std::pair<speed_t, int>> PosixUART::enum2baud_ = {
    {BaudRate::b0, {B0, 0}},
    {BaudRate::b50, {B50, 50}},
    {BaudRate::b75, {B75, 75}},
    {BaudRate::b110, {B110, 110}},
    {BaudRate::b134, {B134, 134}},
    {BaudRate::b150, {B150, 150}},
    {BaudRate::b200, {B200, 200}},
    {BaudRate::b300, {B300, 300}},
    {BaudRate::b600, {B600, 600}},
    {BaudRate::b1200, {B1200, 1200}},
    {BaudRate::b1800, {B1800, 1800}},
    {BaudRate::b2400, {B2400, 2400}},
    {BaudRate::b4800, {B4800, 4800}},
    {BaudRate::b9600, {B9600, 9600}},
    {BaudRate::b19200, {B19200, 19'200}},
    {BaudRate::b38400, {B38400, 38'400}},
    {BaudRate::b57600, {B57600, 57'600}},
    {BaudRate::b115200, {B115200, 115'200}},
    {BaudRate::b230400, {B230400, 230'400}},
    {BaudRate::b460800, {B460800, 460'800}},
    {BaudRate::b921600, {B921600, 921'600}},
    {BaudRate::b1000000, {B1000000, 1'000'000}},
    {BaudRate::b2000000, {B2000000, 2'000'000}},
    {BaudRate::b3000000, {B3000000, 3'000'000}},
    {BaudRate::b4000000, {B4000000, 4'000'000}}};

const std::unordered_map<speed_t, BaudRate> PosixUART::baud2enum_ = {{B0, BaudRate::b0},
                                                                     {B50, BaudRate::b50},
                                                                     {B75, BaudRate::b75},
                                                                     {B110, BaudRate::b110},
                                                                     {B134, BaudRate::b134},
                                                                     {B150, BaudRate::b150},
                                                                     {B200, BaudRate::b200},
                                                                     {B300, BaudRate::b300},
                                                                     {B600, BaudRate::b600},
                                                                     {B1200, BaudRate::b1200},
                                                                     {B1800, BaudRate::b1800},
                                                                     {B2400, BaudRate::b2400},
                                                                     {B4800, BaudRate::b4800},
                                                                     {B9600, BaudRate::b9600},
                                                                     {B19200, BaudRate::b19200},
                                                                     {B38400, BaudRate::b38400},
                                                                     {B57600, BaudRate::b57600},
                                                                     {B115200, BaudRate::b115200},
                                                                     {B230400, BaudRate::b230400},
                                                                     {B460800, BaudRate::b460800},
                                                                     {B921600, BaudRate::b921600},
                                                                     {B1000000, BaudRate::b1000000},
                                                                     {B2000000, BaudRate::b2000000},
                                                                     {B3000000, BaudRate::b3000000},
                                                                     {B4000000, BaudRate::b4000000}};

const std::unordered_map<CharacterSize, std::uint8_t> PosixUART::enum2charsize_ = {{CharacterSize::cs5, CS5},
                                                                                   {CharacterSize::cs6, CS6},
                                                                                   {CharacterSize::cs7, CS7},
                                                                                   {CharacterSize::cs8, CS8}};

const std::unordered_map<std::uint8_t, CharacterSize> PosixUART::charsize2enum_ = {{CS5, CharacterSize::cs5},
                                                                                   {CS6, CharacterSize::cs6},
                                                                                   {CS7, CharacterSize::cs7},
                                                                                   {CS8, CharacterSize::cs8}};

const std::unordered_map<Parity, std::uint8_t> PosixUART::enum2parity_ = {{Parity::none, 0U},
                                                                          {Parity::even, PARENB},
                                                                          {Parity::odd, PARENB | PARODD}};
const std::unordered_map<std::uint8_t, Parity> PosixUART::parity2enum_ = {{0U, Parity::none},
                                                                          {PARENB, Parity::even},
                                                                          {PARENB | PARODD, Parity::odd}};

auto PosixUART::baudrate() const noexcept -> BaudRate {
    auto baud = BaudRate::b9600;
    try {
        baud = baud2enum_.at(baudrate_);
    } catch (const std::out_of_range&) {
        baud = BaudRate::b9600;
    }
    return baud;
}
auto PosixUART::set_baudrate(BaudRate baud) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        baudrate_ = enum2baud_.at(baud).first;
    } catch (std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid baud rate")};
    }
    return configure();
}

auto PosixUART::charactersize() const noexcept -> CharacterSize {
    auto csz = CharacterSize::cs8;
    try {
        csz = charsize2enum_.at(charactersize_);
    } catch (const std::out_of_range&) {
        csz = CharacterSize::cs8;
    }
    return csz;
}
auto PosixUART::set_charactersize(CharacterSize charsize) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        charactersize_ = enum2charsize_.at(charsize);
    } catch (const std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid character size")};
    }
    return configure();
}

auto PosixUART::parity() const noexcept -> Parity {
    auto par = Parity::none;
    try {
        par = parity2enum_.at(parity_);
    } catch (const std::out_of_range&) {
        par = Parity::none;
    }
    return par;
}
auto PosixUART::set_parity(Parity parity) -> std::expected<bool, std::pair<int, std::string>> {
    try {
        parity_ = enum2parity_.at(parity);
    } catch (const std::out_of_range&) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(EINVAL, "Invalid parity")};
    }
    return configure();
}

auto PosixUART::timeout() const noexcept -> std::chrono::milliseconds {
    using namespace std::chrono;
    return duration_cast<milliseconds>(duration<std::uint8_t, std::deci>(timeout_deciseconds_));
}
auto PosixUART::set_timeout(std::chrono::milliseconds timeout_ms) -> std::expected<bool, std::pair<int, std::string>> {
    using namespace std::chrono;
    auto timeout = timeout_ms.count();
    // setting timeout to 0 causes an infinite (or zero) wait, so if a non-zero timeout was specified, the value used
    // should be at least 1
    timeout_deciseconds_ = static_cast<std::uint8_t>((timeout <= 0) ? 0 : std::clamp(timeout / 100L, 1L, 255L));
    return configure();
}

auto PosixUART::open() -> std::expected<bool, std::pair<int, std::string>> {
    if (isopen_) {
        close();
    }
    uarthandle_ = ::open(devicename_.c_str(), O_RDWR | O_NOCTTY);
    if (uarthandle_ < 0) {
        isopen_ = false;
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
    }
    isopen_ = true;
    auto result = configure();
    isopen_ = result.has_value() and result.value();
    if (not isopen_) {
        close();
    }
    return result;
}

// return: true if port is successfully configured
//         error code and string via std::unexpected if configuration failed
auto PosixUART::configure() -> std::expected<bool, std::pair<int, std::string>> {
    if (not isopen_) {
        return false;
    }
    // capture port settings
    auto tios = termios{0};
    auto result = tcgetattr(uarthandle_, &tios);
    if (result != 0) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
    }
    /// TODO: higher baud rates need a different process?
    tios.c_cflag = baudrate_ | charactersize_ | parity_ | stopbits_ | CLOCAL | CREAD;
    tios.c_iflag = IGNPAR | INPCK;
    tios.c_oflag = 0;
    tios.c_lflag = 0;  // non-canonical, no echo, ...
    tios.c_cc[VTIME] = timeout_deciseconds_;
    tios.c_cc[VMIN] = 0;
    result = tcflush(uarthandle_, TCIOFLUSH);
    if (result != 0) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
    }
    result = tcsetattr(uarthandle_, TCSANOW, &tios);
    if (result != 0) {
        return std::unexpected<std::pair<int, std::string>>{std::make_pair(errno, std::strerror(errno))};
    }
    currenttio_ = tios;
    return true;
}

void PosixUART::close() {
    if (isopen_) {
        ::close(uarthandle_);
    }
}

}  // namespace uart
