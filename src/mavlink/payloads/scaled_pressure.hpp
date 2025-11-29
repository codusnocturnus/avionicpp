#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Scaled Pressure (Msg ID 29).
/// @details The pressure readings for the typical setup of one absolute and differential pressure sensor. The units are
/// as specified in each field.
template <typename Traits>
struct ScaledPressure_T {
    static constexpr std::uint32_t MessageId = 29;
    static constexpr std::string_view MessageName = "SCALED_PRESSURE";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<float> press_abs;             ///< Absolute pressure
    typename Traits::template Field<float> press_diff;            ///< Differential pressure 1
    typename Traits::template Field<std::int16_t> temperature;    ///< Absolute pressure temperature

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<ScaledPressure_T<Traits>>(); }
};

using ScaledPressure = ScaledPressure_T<TxTraits>;
using LazyScaledPressure = ScaledPressure_T<RxTraits>;

}  // namespace mavlink::payloads
