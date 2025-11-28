# Mavlink Message Handling Context

## 1. Core Architecture Overview

The Mavlink implementation follows a similar design philosophy to the NMEA-0183 handler, focusing on **Type-Safety**, **Zero-Allocation**, and **Reflection-Based** processing.

*   **Traits-Based Payloads:** Message payloads are defined as templates (e.g., `Heartbeat_T<Traits>`). This allows swapping field behavior for sending (Tx) and receiving (Rx).
*   **Reflection:** The library uses `boost::pfr` to iterate over payload struct members automatically. This enables generic serialization, deserialization, and even compile-time CRC_EXTRA calculation without repetitive boilerplate code.
*   **Enumerations as Namespaces:** Enumerations are implemented as namespaces containing `constexpr` values (e.g., `namespace MavType { constexpr uint8_t FIXED_WING = 1; }`) rather than `enum class`. This simplifies bitwise operations and avoids casting in usage.

## 2. Serialization (Tx)

Serialization converts a payload struct into a Mavlink v2 frame byte sequence.

*   **`serialize()` Function:**
    *   **Zero-Allocation:** Writes directly to a provided output buffer (e.g., `std::span`), returning the number of bytes written. It avoids `std::vector` or dynamic allocation during runtime.
    *   **Reflection:** Uses `boost::pfr::for_each_field` to iterate over the payload struct members.
    *   **Copying:** Uses `std::copy` (via `get_value` helper) to copy field values into the buffer. This assumes the struct layout is compatible with the wire format (fields sorted by size).
    *   **Zero Truncation:** Automatically truncates trailing zero bytes from the payload (Mavlink v2 optimization) before writing the header length.
    *   **CRC Calculation:** Computes the X.25 CRC over the header and payload, and importantly, accumulates the message-specific `CrcExtra` byte.

## 3. Deserialization (Rx)

Deserialization consists of framing the byte stream and then mapping the payload.

*   **Framer:** A coroutine-based state machine (`create_framer`) that yields `MessageView` objects.
    *   Supports both **Mavlink v1** (0xFE) and **Mavlink v2** (0xFD) headers transparently.
    *   Parses header fields incrementally during reception.
    *   Yields a `MessageView` containing the header info and a `std::span` of the payload.
*   **`deserialize()` Function:**
    *   Takes a `MessageView` and returns a populated payload struct.
    *   Uses `boost::pfr::for_each_field` to iterate over the target struct fields.
    *   Copies bytes from the payload span into the struct fields.
    *   **Zero Padding:** Handles zero-truncation by zero-initializing remaining fields if the received payload is shorter than the struct (Mavlink 2 feature).

## 4. Checksum & CRC Extra

Mavlink ensures message definition consistency using a `CRC_EXTRA` byte seeded into the checksum.

*   **Compile-Time Calculation:** `src/mavlink/checksum.hpp` provides a `consteval` function `calculate_crc_extra<MessageT>()`.
    *   It uses `boost::pfr` to reflect on the message struct *at compile time*.
    *   It extracts field names and types (mapped via `type_string` helper).
    *   It computes the CRC_EXTRA based on the standard Mavlink algorithm (message name + field types/names).
*   **Usage:** The `CrcExtra` constant is defined as a `static constexpr` member in the payload struct, ensuring correct CRC calculation without manual maintenance.

## 5. Supported Messages

The library currently supports the following Mavlink messages:

*   **Heartbeat:** (Msg ID 0)
*   **SysStatus:** (Msg ID 1)
*   **SystemTime:** (Msg ID 2)
*   **ChangeOperatorControl:** (Msg ID 5)
*   **ChangeOperatorControlAck:** (Msg ID 6)
*   **AuthKey:** (Msg ID 7)
*   **ParamRequestRead:** (Msg ID 20)
*   **ParamRequestList:** (Msg ID 21)
*   **ParamValue:** (Msg ID 22)
*   **ParamSet:** (Msg ID 23)
*   **GpsRawInt:** (Msg ID 24)
*   **GpsStatus:** (Msg ID 25)
*   **ScaledImu:** (Msg ID 26)
*   **RawImu:** (Msg ID 27)
*   **RawPressure:** (Msg ID 28)
*   **ScaledPressure:** (Msg ID 29)
*   **Attitude:** (Msg ID 30)
*   **AttitudeQuaternion:** (Msg ID 31)
*   **LocalPositionNed:** (Msg ID 32)
*   **GlobalPositionInt:** (Msg ID 33)
*   **RcChannelsScaled:** (Msg ID 34)
*   **RcChannelsRaw:** (Msg ID 35)
*   **VfrHud:** (Msg ID 74)
*   **CommandInt:** (Msg ID 75)
*   **CommandLong:** (Msg ID 76)
*   **CommandAck:** (Msg ID 77)
*   **AutopilotVersion:** (Msg ID 148)

## 6. Payload Definition Example

```cpp
namespace mavlink::payloads {

/// @brief Heartbeat (Msg ID 0).
template <typename Traits>
struct Heartbeat_T {
    static constexpr std::uint32_t MessageId = 0;
    static constexpr std::string_view MessageName = "HEARTBEAT";

    // Fields sorted by size (Mavlink requirement for wire compatibility)
    typename Traits::template Field<std::uint32_t> custom_mode;
    typename Traits::template Field<std::uint8_t> type;       // uses MavType namespace values
    typename Traits::template Field<std::uint8_t> autopilot;  // uses MavAutopilot namespace values
    typename Traits::template Field<std::uint8_t> base_mode;
    typename Traits::template Field<std::uint8_t> system_status;
    typename Traits::template Field<std::uint8_t> mavlink_version;

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<Heartbeat_T<Traits>>();
};

using Heartbeat = Heartbeat_T<TxTraits>;
using LazyHeartbeat = Heartbeat_T<RxTraits>;

}
