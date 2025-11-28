#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief Type of GPS fix
namespace MavGpsFixType {

constexpr std::uint8_t NO_GPS = 0;     ///< No GPS connected
constexpr std::uint8_t NO_FIX = 1;     ///< No position information, GPS is connected
constexpr std::uint8_t FIX_2D = 2;     ///< 2D position
constexpr std::uint8_t FIX_3D = 3;     ///< 3D position
constexpr std::uint8_t DGPS = 4;       ///< DGPS/SBAS aided 3D position
constexpr std::uint8_t RTK_FLOAT = 5;  ///< RTK float, 3D position
constexpr std::uint8_t RTK_FIXED = 6;  ///< RTK Fixed, 3D position
constexpr std::uint8_t STATIC = 7;     ///< Static fixed, typically used for base stations
constexpr std::uint8_t PPP = 8;        ///< PPP, 3D position

}  // namespace MavGpsFixType

}  // namespace mavlink::enumerations
