# UART Processing Context

## 1. Core Architecture Overview

The UART library is designed around **Static Polymorphism** and **Dependency Injection** to provide a flexible, zero-overhead interface for serial communication across different platforms.

*   **Template-Based Wrapper:** The core `UART<UARTType>` class acts as a lightweight wrapper around a concrete implementation (e.g., `Win32UART`, `PosixUART`).
*   **Static Polymorphism:** By using templates instead of virtual functions (inheritance), the library avoids vtable overhead and enables potential compiler optimizations (inlining).
*   **Dependency Injection:** The concrete implementation type is injected via the template parameter. This facilitates easy swapping of implementations for cross-platform builds (Windows/Linux) and testing (Mock/Stub).

## 2. Platform Implementations

The library provides specific implementations for supuarted operating systems, managed via CMake configuration.

*   **`Win32UART` (Windows):**
    *   Uses the Windows API (`CreateFile`, `ReadFile`, `WriteFile`, `GetCommState`, `SetCommState`).
    *   Handles `DCB` configuration for baud rate, parity, etc.
    *   Manages timeouts via `COMMTIMEOUTS`.
    *   Uses `DWORD` for buffer sizes, with logic to safely handle `size_t` conversions.
*   **`PosixUART` (Linux/Unix):**
    *   Uses POSIX standard API (`open`, `read`, `write`, `tcgetattr`, `tcsetattr`).
    *   Configures terminal options using `termios` structure.
    *   Maps standard baud rates to `Bxxxx` constants.
    *   Handles `ssize_t` return values and `errno` for error reuarting.
*   **`StubUART` (Testing):**
    *   A no-op implementation useful for unit testing and development without hardware.
    *   Simulates success for all operations to allow logic testing of higher-level components.

## 3. Configuration

Configuration is type-safe and uses scoped enumerations defined in `settings.hpp`.

*   **Parameters:**
    *   `BaudRate`: Strongly typed enum covering standard rates (e.g., `BaudRate::b9600`, `BaudRate::b115200`).
    *   `CharacterSize`: Data bits (5-8, 16).
    *   `Parity`: None, Even, Odd, Mark, Space.
    *   `StopBits`: 1, 1.5, 2.
*   **Setters:**
    *   Configuration setters (e.g., `set_baudrate`) return `std::expected<bool, std::pair<int, std::string>>`.
    *   This allows callers to verify if the requested configuration was successfully applied by the underlying OS driver.

## 4. Communication (Read/Write)

Read and write operations leverage C++20 Ranges for modern, safe buffer handling.

*   **Interface:**
    *   `read(std::ranges::range auto& buffer, size_t readsize)`
    *   `write(const std::ranges::range auto& buffer)`
*   **Implementation:**
    *   Accepts any contiguous range (e.g., `std::span`, `std::vector`, `std::array`).
    *   Returns `std::expected<size_t, std::pair<int, std::string>>`, providing either the number of bytes transferred or an error code/message.
    *   **Zero-Copy (Interface):** The wrapper passes references to the underlying implementation, avoiding unnecessary data copying.

## 5. Error Handling

The library eschews exceptions in favor of `std::expected` for runtime errors, aligning with modern C++ error handling practices.

*   **Return Values:** Operations returning status use `std::expected`.
    *   Success: Contains the result (e.g., `size_t` bytes read) or `true`.
    *   Failure: Contains a `std::pair<int, std::string>` representing the system error code (errno/GetLastError) and a descriptive message.
*   **Safety:**
    *   Constructors are `explicit` to prevent accidental implicit conversions.
    *   Implementations safeguard against integer overflows when converting between `size_t` and OS-specific types (`DWORD`, `ssize_t`).

## 6. Thread Safety

*   **Not Thread-Safe:** The `UART` class and its implementations are **not thread-safe**.
*   **Synchronization:** If an instance is shared across multiple threads (e.g., one thread reading, another writing), external synchronization (e.g., `std::mutex`) is required to prevent race conditions on internal handles and state.

## 7. Usage Example

```cpp
#include "uart/uart.hpp"
#if defined(_WIN32)
    #include "uart/win32uart.hpp"
    using SerialPort = uart::UART<uart::Win32UART>;
#else
    #include "uart/posixuart.hpp"
    using SerialPort = uart::UART<uart::PosixUART>;
#endif

void example() {
    SerialPort serial("/dev/ttyUSB0"); // or "COM3" on Windows

    // Open uart
    if (auto result = serial.open(); !result) {
        // Handle error: result.error().second
        return;
    }

    // Configure
    if (!serial.set_baudrate(uart::BaudRate::b115200)) {
        // Handle configuration error
    }

    // Write data
    std::array<uint8_t, 5> data = {0xAA, 0xBB, 0xCC};
    if (auto res = serial.write(data); res) {
        // *res bytes written
    }

    // Read data
    std::vector<uint8_t> buffer(1024);
    if (auto res = serial.read(buffer, 100); res) {
        // *res bytes read
    }
}
