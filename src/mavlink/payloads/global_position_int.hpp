#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Global Position Int (Msg ID 33).
/// @details The filtered global position (e.g. fused GPS and accelerometers). The position is in GPS-frame
/// (right-handed, Z-up). It is designed as scaled integer message since the resolution of float is not sufficient.
template <typename Traits>
struct GlobalPositionInt_T {
    static constexpr std::uint32_t MessageId = 33;
    static constexpr std::string_view MessageName = "GLOBAL_POSITION_INT";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<std::int32_t> lat;            ///< Latitude, expressed as degrees * 1E7.
    typename Traits::template Field<std::int32_t> lon;            ///< Longitude, expressed as degrees * 1E7.
    typename Traits::template Field<std::int32_t>
        alt;  ///< Altitude in meters, expressed as * 1000 (millimeters), AMSL.
    typename Traits::template Field<std::int32_t>
        relative_alt;  ///< Altitude above ground in meters, expressed as * 1000 (millimeters).
    typename Traits::template Field<std::int16_t>
        vx;  ///< Ground X Speed (Latitude, positive north), expressed as m/s * 100.
    typename Traits::template Field<std::int16_t>
        vy;  ///< Ground Y Speed (Longitude, positive east), expressed as m/s * 100.
    typename Traits::template Field<std::int16_t>
        vz;  ///< Ground Z Speed (Altitude, positive down), expressed as m/s * 100.
    typename Traits::template Field<std::uint16_t>
        hdg;  ///< Vehicle heading (yaw angle) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<GlobalPositionInt_T<Traits>>(); }
};

using GlobalPositionInt = GlobalPositionInt_T<TxTraits>;
using LazyGlobalPositionInt = GlobalPositionInt_T<RxTraits>;

}  // namespace mavlink::payloads
