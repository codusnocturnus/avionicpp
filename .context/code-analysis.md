# Code Analysis: Safety, Security, and Resource Usage

## Safety and Security

The library demonstrates a strong focus on safety and correctness through modern C++ practices.

*   **Buffer Safety:** The codebase consistently uses safe abstractions such as `std::span` and `std::string_view` instead of raw pointers. Bounds checking is performed explicitly (e.g., in `framer.hpp` to prevent buffer overruns) and implicitly via standard library algorithms (`std::format_to_n`, `std::copy`). `FixedString` operations use compile-time sizes to ensure safe copying. Serialization uses `std::span` and explicitly checks buffer size against the maximum Mavlink packet size (280 bytes). Deserialization checks available bytes before copying.
*   **Input Validation:**
    *   **Protocol Integrity:**
        *   **NMEA:** Validates checksums (XOR) of every message.
        *   **Mavlink:** Calculates CRC-16-CCITT with `CRC_EXTRA` seed (derived from message definition) to ensure definition consistency between sender and receiver.
    *   **Parsing Safety:**
        *   **NMEA:** Field parsing relies on `std::from_chars`, checking error codes.
        *   **Mavlink:** Uses type-safe binary deserialization. `boost::pfr` reflection ensures structure fields are populated correctly. Deserialization handles truncated payloads (Mavlink 2 zero-byte truncation) by zero-filling missing fields.
*   **Type Safety:** The extensive use of strong typing (Scoped Enums/Namespaces, `TxField` templates, `std::chrono` types) prevents common errors.

## Heap Allocations and Real-Time Suitability

The library is designed with embedded and real-time constraints in mind, aiming for zero dynamic allocation during steady-state operation.

### 1. Serialization and Deserialization (Steady State)
*   **Zero Allocation:** The core `serializer.hpp` and `deserializer.hpp` components (both NMEA and Mavlink) are designed to operate without any heap allocations.
*   **Mechanism:**
    *   **NMEA:** Uses stack-based formatting wrappers.
    *   **Mavlink:** Uses direct memory operations (`std::copy`) into provided buffers (`std::span`). It specifically avoids `std::vector` return types for serialization.
    *   **Reflection:** `boost::pfr` operates at compile-time/stack-time, incurring no allocation overhead.

### 2. Framer (Initialization)
*   **Allocation at Startup:** The `framer.hpp` components use C++20 coroutines.
*   **Details:** When a coroutine is initially called, the compiler allocates memory for the coroutine state (promise, parameters, and local variables) on the heap.
*   **Real-Time Context:** This allocation happens **once** when the framer is initialized (usually at program startup). Once the framer is running (in its `while(true)` loop), it yields and resumes execution using the pre-allocated state, incurring **no further heap allocations**.
*   **Note for Developers:** Application developers should ensure that framer initialization occurs during the system startup phase, outside of critical real-time loops.

## Exception Handling

The library is largely designed to be exception-free during normal operation, relying on `std::expected` and `std::optional` for error handling.

*   **Return Types:** Functions return `std::expected<T, Error>` or `std::optional<T>` to propagate errors explicitly.
*   **Memory Allocation:** The only potential source of `std::bad_alloc` is the coroutine initialization in `framer.hpp`.
*   **Standard Library:** The code uses non-throwing standard library features (views, arrays, spans) where possible.

## Coding Guidelines Adherence

*   **C++ Standard:** Code compiles with C++23 (GCC 14).
*   **Modern C++:**
    *   **AAA (Almost Always Auto):** Applied consistently in Mavlink and NMEA refactoring.
    *   **Constexpr/Consteval:** Used extensively, including complex compile-time CRC calculations for Mavlink (`calculate_crc_extra`).
*   **Enumerations:** Both Mavlink and NMEA libraries use `namespace` with `constexpr` values instead of `enum class` to simplify usage (avoiding casts) while maintaining grouping.
*   **Naming Conventions:**
    *   General adherence to `PascalCase` for types and `lower_snake_case` for functions/variables.
    *   **Deviation:** Mavlink enumeration values (e.g., `MavType::FIXED_WING`) use `UPPER_CASE`. This deviates from the "PascalCase constants" guideline but aligns with the strong "business domain" convention of the Mavlink protocol definition. NMEA enumerations (e.g. `DirectionIndicator::North`) follow the PascalCase guideline.

## Potential Issues & Limitations
*   **Endianness:** The Mavlink implementation (serialization/deserialization) assumes a Little Endian host architecture (using `reinterpret_cast` and `std::copy`). This is standard for ARM and x86 but would require modification for Big Endian systems.
*   **Mavlink Extensions:** While the deserializer handles truncation (zero-padding), the `calculate_crc_extra` function currently iterates over *all* fields in the struct. To strictly comply with Mavlink 2 CRC rules, extension fields should ideally be excluded from CRC calculation. The current implementation typically omits extension fields from the payload structs to avoid incorrect CRC calculation, which means extension data is currently not accessible for those messages.
