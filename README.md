# avionicpp

[![Linux Clang](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_linux_clang.yml/badge.svg)](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_linux_clang.yml)
[![Linux GCC](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_linux_gcc.yml/badge.svg)](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_linux_gcc.yml)
[![Windows MSVC](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_windows_msvc.yml/badge.svg)](https://github.com/codusnocturnus/avionicpp/actions/workflows/ci_windows_msvc.yml)
[![Coverage Status](https://coveralls.io/repos/github/codusnocturnus/avionicpp/badge.svg?branch=main)](https://coveralls.io/github/codusnocturnus/avionicpp?branch=main)

**avionicpp** is a modern C++ library designed for communicating with common avionics devices and processing their data efficiently. It leverages the latest C++ standards (C++20/23) to provide a type-safe, zero-allocation, and high-performance interface for protocols like MAVLink and NMEA-0183, as well as cross-platform UART communication.

Note that the vast majority of the code has been produced incrementally by LLM's (specifically Gemini 3 Pro) with careful prompting and review to keep the architecture and design consistent.

## Key Features

### 🚀 Modern C++ Design
Built from the ground up using modern C++ paradigms:
*   **Concepts & Ranges:** for clear interfaces and safe buffer manipulation
*   **`std::expected`:** for robust, exception-free error handling
*   **Compile-Time Reflection:** uses `boost::pfr` to automate serialization and eliminate boilerplate

### 📡 MAVLink Support
A high-performance MAVLink implementation focusing on safety and speed:
*   **Zero-Allocation:** serializes directly to output buffers without dynamic allocation
*   **Reflection-Based:** automatic field iteration for serialization and CRC calculation
*   **Type-Safe:** uses strong types and namespaces for enumerations (no raw `int` constants)
*   **Dual Version:** transparent support for both MAVLink v1 and v2 framing

### 🧭 NMEA-0183 Support
Efficient parsing and generation of NMEA sentences:
*   **Zero-Copy / Lazy Evaluation:** parser yields views into the buffer; fields are parsed only when accessed
*   **Compile-Time Formatting:** type-safe serialization with compile-time precision and width specifiers
*   **Customizable Traits:** single struct definitions for both Rx and Tx via trait swapping

### 🔌 Cross-Platform UART
A flexible serial communication layer:
*   **Static Polymorphism:** template-based dependency injection avoids virtual function overhead
*   **Platform Specifics:** native implementations for **Windows** (`Win32UART`) and **Linux** (`PosixUART`)
*   **Mockable:** Includes `StubUART` for easy unit testing without hardware

##  Future Development
- specialized handlers for hobby grade devices like IMU's, pressure sensors, servos, and maybe the U-Blox protocol
- higher level constructs and algorithms for combining measurements and making use of actuators

## Performance and Benchmarking
Effort is made to follow the "zero-cost" model of modern C++ features, as well as constructing a cohesive library with consistent interfaces that can be easily reasoned about at any level. Where appropriate and possible, benchmarks will be used to compare the performance of this library's implementation with reference implementations.

## Requirements

*   **C++ Compiler:** C++23 compliant (GCC 13+, Clang 16+, MSVC 19.36+).
*   **CMake:** Version 3.25 or later.
*   **Dependencies:** (all fetched automatically via CMake)
    *   [Boost.PFR](https://github.com/boostorg/pfr)
    *   [Catch2](https://github.com/catchorg/Catch2) (for unit tests)
    *   [FakeIt](https://github.com/eranpeer/FakeIt.git) (for test mocks)
    *   [Google Benchmark](https://github.com/google/benchmark.git) (for running benchmarks)

## Installation

You can integrate `avionicpp` into your project using CMake FetchContent.

### Building from Source

```bash
git clone https://github.com/codusnocturnus/avionicpp.git
cd avionicpp
cmake -S . -B build
cmake --build build
```

### Running Tests

```bash
ctest --test-dir build --output-on-failure
```

## Usage Examples

### UART Communication

```cpp
#include "uart/uart.hpp"
#if defined(_WIN32)
    #include "uart/win32uart.hpp"
    using SerialPort = uart::UART<uart::Win32UART>;
#else
    #include "uart/posixuart.hpp"
    using SerialPort = uart::UART<uart::PosixUART>;
#endif

int main() {
    SerialPort serial("/dev/ttyUSB0");

    if (auto res = serial.open(); !res) {
        // Handle error: res.error().second
        return -1;
    }

    serial.set_baudrate(uart::BaudRate::b115200);

    std::vector<uint8_t> buffer(1024);
    if (auto res = serial.read(buffer, 100); res) {
        // Process *res bytes...
    }
}
```

### MAVLink Serialization

```cpp
#include "mavlink/payloads/heartbeat.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

void send_heartbeat() {
    Heartbeat hb;
    hb.custom_mode.value = 0;
    hb.type.value = MavType::QUADROTOR;
    hb.autopilot.value = MavAutopilot::ARDUPILOTMEGA;
    hb.base_mode.value = MavModeFlag::MANUAL_INPUT_ENABLED;
    hb.system_status.value = MavState::STANDBY;
    hb.mavlink_version.value = 3;

    std::array<uint8_t, 256> buffer;
    // Serialize: payload, sys_id, comp_id, seq, buffer
    auto len = serialize(hb, 1, 1, 0, buffer);
    
    // 'buffer' now contains the full MAVLink frame
}
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1.  Fork the repository.
2.  Create your feature branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

## License

This project is licensed under the BSD 3-Clause License - see the [LICENSE](LICENSE) file for details.
