#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Raw Pressure (Msg ID 28).
/// @details The RAW pressure readings for the typical setup of one absolute pressure and one differential pressure
/// sensor. The sensor values should be the raw, UNSCALED ADC values.
template <typename Traits>
struct RawPressure_T {
    static constexpr std::uint32_t MessageId = 28;
    static constexpr std::string_view MessageName = "RAW_PRESSURE";

    typename Traits::template Field<std::uint64_t>
        time_usec;  ///< Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp
                    ///< format (since 1.1.1970 or since system boot) by checking for the magnitude of the number.
    typename Traits::template Field<std::int16_t> press_abs;    ///< Absolute pressure (raw)
    typename Traits::template Field<std::int16_t> press_diff1;  ///< Differential pressure 1 (raw, 0 if nonexistent)
    typename Traits::template Field<std::int16_t> press_diff2;  ///< Differential pressure 2 (raw, 0 if nonexistent)
    typename Traits::template Field<std::int16_t> temperature;  ///< Raw Temperature measurement (raw)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<RawPressure_T<Traits>>();
};

using RawPressure = RawPressure_T<TxTraits>;
using LazyRawPressure = RawPressure_T<RxTraits>;

}  // namespace mavlink::payloads
