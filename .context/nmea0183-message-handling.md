# NMEA-0183 Message Handling Context

## 1. Core Architecture Overview

The system is designed around a **Zero-Copy / Lazy-Evaluation** philosophy for deserialization and a **Type-Safe / Zero-Allocation** approach for serialization.

*   **Traits-Based Payloads:** Message payloads are defined as templates (e.g., `ROT_T<Traits>`). This allows the same struct definition to be used for both sending (Tx) and receiving (Rx) by swapping the underlying field types via `TxTraits` or `RxTraits`.
*   **Reflection:** The library uses `boost::pfr` to iterate over struct members automatically, eliminating the need for manual mapping code in both serialization and deserialization.

## 2. Serialization (Tx)

Serialization is focused on supporting optional fields and precise formatting efficiently without heap allocations.

*   **`TxField<T, Precision, Width>`:** A wrapper for values to be sent. It holds the value in a `std::optional<T>`, allowing fields to be explicitly empty. It carries compile-time `Precision` (decimal places for floats) and `Width` (zero-padding width for integers/floats).
*   **Zero-Allocation Formatting:**
    *   **Custom Formatter:** Formatting is handled by a specialization of `std::formatter` for a lightweight `OptionalWrapper` type. This allows the serialization logic to write directly to the output buffer using `std::format_to`.
    *   **Formatting Logic:**
        *   If the value is present, it formats the value respecting `Precision` and `Width`. For example, `TxField<int, 0, 2>` formats `1` as `01`.
        *   If the value is empty, it writes nothing (resulting in empty NMEA fields like `,,`).
*   **`serialize()` Function:**
    1.  **Tuple Conversion:** Uses `boost::pfr::structure_to_tuple` to convert the payload struct into a tuple of fields.
    2.  **Formatting:** Uses `std::format_to_n` with the generated format string (generic `${}{},{},{}`). The `to_formattable` helper converts each `TxField` into an `OptionalWrapper` for the formatter.
    3.  **Checksum:** Calculates the XOR checksum of the formatted payload in-place.
    4.  **Footer:** Appends the checksum and `\r\n`.

## 3. Deserialization (Rx)

Deserialization is split into two stages: **Framing** and **Binding**.

*   **Framer:** Coroutine-based state machine that yields `MessageView` objects (zero-copy).
*   **Binder:** Maps `MessageView` fields to struct members.
*   **`RxField<T>` (Lazy):** Stores a `std::string_view` token. Parses only on demand via `value()`. Handles empty tokens as `std::nullopt`.

## 4. Helpers and Utilities

*   **`utilities.hpp`**: Provides higher-level conversion templates to work with standard C++ types, while keeping the serialization layer simple (1:1 field mapping).
    *   `get_timestamp` / `set_timestamp`: Converts between NMEA time/date fields and `std::chrono::utc_clock::time_point`. Supports standard (Day/Month/Year) and RMC (Date) formats.
    *   `get_latitude_deg` / `set_latitude_deg`: Converts between NMEA Latitude (`ddmm.mm` + `N/S`) and decimal degrees (double).
    *   `get_longitude_deg` / `set_longitude_deg`: Converts between NMEA Longitude (`dddmm.mm` + `E/W`) and decimal degrees (double).

## 5. Supported Messages

*   **DTM:** Datum Reference.
*   **GBS:** GNSS Satellite Fault Detection (UTC Time, Error estimates, Bias, StdDev).
*   **GGA:** Global Positioning System Fix Data (Time, Pos, Qual, Sats, HDOP, Alt, Geoid).
*   **GLL:** Geographic Position - Latitude/Longitude (Lat, Lon, Time, Status, Mode).
*   **GNS:** GNSS Fix Data (Time, Pos, Mode, Sats, HDOP, Alt, Geoid, DGPS, NavStatus).
*   **GSA:** GNSS DOP and Active Satellites (Mode, IDs, PDOP, HDOP, VDOP).
*   **GST:** GNSS Pseudorange Error Statistics (Time, RMS, Error Ellipse, StdDevs).
*   **HDT:** Heading True.
*   **RMC:** Recommended Minimum Navigation Information (Time, Status, Pos, Speed, Course, Date, MagVar, Mode).
*   **ROT:** Rate of Turn.
*   **VTG:** Course Over Ground and Ground Speed (Course True/Mag, Speed Knots/Kph, Mode).
*   **ZDA:** Time & Date (UTC, Day, Month, Year, Local Zone).

## 6. Payload Definition Example

```cpp
template <typename Traits>
struct ZDA_T {
    static constexpr std::string_view messageid = "ZDA"sv;
    // Float with 2 decimals, zero-padded to 9 chars (hhmmss.ss)
    typename Traits::template Float<double, 2, 9> utc_time;
    // Integer zero-padded to 2 chars
    typename Traits::template Int<int, 2> day;
    // ...
};
