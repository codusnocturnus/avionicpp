#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief VFR HUD (Msg ID 74).
/// @details Metrics typically displayed on a HUD for fixed wing aircraft.
template <typename Traits>
struct VfrHud_T {
    static constexpr std::uint32_t MessageId = 74;
    static constexpr std::string_view MessageName = "VFR_HUD";

    typename Traits::template Field<float> airspeed;     ///< Current airspeed in m/s.
    typename Traits::template Field<float> groundspeed;  ///< Current ground speed in m/s.
    typename Traits::template Field<float> alt;          ///< Current altitude (MSL), in meters.
    typename Traits::template Field<float> climb;        ///< Current climb rate in m/s.
    typename Traits::template Field<std::int16_t>
        heading;  ///< Current heading in degrees, in compass units (0..360, 0=north).
    typename Traits::template Field<std::uint16_t>
        throttle;  ///< Current throttle setting in integer percent, 0 to 100.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<VfrHud_T<Traits>>(); }
};

using VfrHud = VfrHud_T<TxTraits>;
using LazyVfrHud = VfrHud_T<RxTraits>;

}  // namespace mavlink::payloads
