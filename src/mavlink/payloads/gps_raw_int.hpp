#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_gps_fix_type.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief GPS Raw Int (Msg ID 24).
/// @details The global position, as returned by the Global Positioning System (GPS). This is NOT the global position
/// estimate of the system, but rather a RAW sensor value. See message GLOBAL_POSITION_INT for the global position
/// estimate.
template <typename Traits>
struct GpsRawInt_T {
    static constexpr std::uint32_t MessageId = 24;
    static constexpr std::string_view MessageName = "GPS_RAW_INT";

    typename Traits::template Field<std::uint64_t>
        time_usec;                                      ///< Timestamp (UNIX Epoch time or time since system boot).
    typename Traits::template Field<std::int32_t> lat;  ///< Latitude (WGS84), in 1E7 degrees.
    typename Traits::template Field<std::int32_t> lon;  ///< Longitude (WGS84), in 1E7 degrees.
    typename Traits::template Field<std::int32_t> alt;  ///< Altitude (MSL). Positive for up.
    typename Traits::template Field<std::int32_t>
        alt_ellipsoid;  ///< Altitude (above WGS84, EGM96 ellipsoid). Positive for up.
    typename Traits::template Field<std::uint32_t> h_acc;    ///< Position uncertainty.
    typename Traits::template Field<std::uint32_t> v_acc;    ///< Altitude uncertainty.
    typename Traits::template Field<std::uint32_t> vel_acc;  ///< Speed uncertainty.
    typename Traits::template Field<std::uint32_t> hdg_acc;  ///< Heading / track uncertainty.
    typename Traits::template Field<std::uint16_t>
        eph;  ///< GPS HDOP horizontal dilution of position (unitless). If unknown, set to: UINT16_MAX.
    typename Traits::template Field<std::uint16_t>
        epv;  ///< GPS VDOP vertical dilution of position (unitless). If unknown, set to: UINT16_MAX.
    typename Traits::template Field<std::uint16_t>
        vel;  ///< GPS ground speed (m/s * 100). If unknown, set to: UINT16_MAX.
    typename Traits::template Field<std::uint16_t>
        cog;  ///< Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If
              ///< unknown, set to: UINT16_MAX.
    typename Traits::template Field<std::uint16_t>
        yaw;  ///< Yaw in earth frame from north. Use 0 if this GPS does not provide yaw. Use 65535 if this GPS is
              ///< configured to provide yaw and is currently unable to provide it. Use 36000 for north.
    typename Traits::template Field<std::uint8_t> fix_type;  ///< GPS fix type (see GPS_FIX_TYPE enum).
    typename Traits::template Field<std::uint8_t>
        satellites_visible;  ///< Number of satellites visible. If unknown, set to 255.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<GpsRawInt_T<Traits>>();
};

using GpsRawInt = GpsRawInt_T<TxTraits>;
using LazyGpsRawInt = GpsRawInt_T<RxTraits>;

}  // namespace mavlink::payloads
