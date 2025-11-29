#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief GPS Status (Msg ID 25).
/// @details The positioning status, as reported by GPS. This message is intended to display status information about
/// each satellite visible to the receiver. See message GLOBAL_POSITION_INT for the global position estimate. This
/// message can contain information for up to 20 satellites.
template <typename Traits>
struct GpsStatus_T {
    static constexpr std::uint32_t MessageId = 25;
    static constexpr std::string_view MessageName = "GPS_STATUS";

    typename Traits::template Field<std::uint8_t> satellites_visible;             ///< Number of satellites visible
    typename Traits::template Field<std::array<std::uint8_t, 20>> satellite_prn;  ///< Global satellite ID
    typename Traits::template Field<std::array<std::uint8_t, 20>>
        satellite_used;  ///< 0: Satellite not used, 1: used for localization
    typename Traits::template Field<std::array<std::uint8_t, 20>>
        satellite_elevation;  ///< Elevation (0: right on top of receiver, 90: on the horizon) of satellite
    typename Traits::template Field<std::array<std::uint8_t, 20>>
        satellite_azimuth;  ///< Direction of satellite, 0: 0 deg, 255: 360 deg.
    typename Traits::template Field<std::array<std::uint8_t, 20>>
        satellite_snr;  ///< Signal to noise ratio of satellite

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<GpsStatus_T<Traits>>(); }
};

using GpsStatus = GpsStatus_T<TxTraits>;
using LazyGpsStatus = GpsStatus_T<RxTraits>;

}  // namespace mavlink::payloads
