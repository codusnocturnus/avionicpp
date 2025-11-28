#pragma once
#include <cstdint>

namespace mavlink::enumerations {

namespace MavFrame {
/// @brief Coordinate frames used by MAVLink. Not all frames are supported by all commands, messages, or vehicles.
/// Global frames use the following naming conventions: "GLOBAL": Global coordinate frame with WGS84 latitude/longitude
/// and altitude positive over mean sea level (MSL) by default. The following modifiers may be used with "GLOBAL":
/// "RELATIVE_ALT": Altitude is relative to the vehicle home position rather than MSL. "TERRAIN_ALT": Altitude is
/// relative to ground level rather than MSL. "INT": Latitude/longitude (in degrees) are scaled by multiplying by 1E7.
/// Local frames use the following naming conventions: "LOCAL": Origin of local frame is fixed relative to earth. Unless
/// otherwise specified this origin is the origin of the vehicle position-estimator ("EKF"). "BODY": Origin of local
/// frame travels with the vehicle. NOTE, "BODY" does NOT indicate alignment of frame axis with vehicle attitude.
/// "OFFSET": Deprecated synonym for "BODY" (origin travels with the vehicle). Not to be used for new frames. Some
/// deprecated frames do not follow these conventions (e.g. MAV_FRAME_BODY_NED and MAV_FRAME_BODY_OFFSET_NED).
constexpr std::uint8_t GLOBAL = 0;
constexpr std::uint8_t LOCAL_NED = 1;
constexpr std::uint8_t MISSION = 2;
constexpr std::uint8_t GLOBAL_RELATIVE_ALT = 3;
constexpr std::uint8_t LOCAL_ENU = 4;
constexpr std::uint8_t GLOBAL_INT = 5;
constexpr std::uint8_t GLOBAL_RELATIVE_ALT_INT = 6;
constexpr std::uint8_t LOCAL_OFFSET_NED = 7;
constexpr std::uint8_t BODY_NED = 8;
constexpr std::uint8_t BODY_OFFSET_NED = 9;
constexpr std::uint8_t GLOBAL_TERRAIN_ALT = 10;
constexpr std::uint8_t GLOBAL_TERRAIN_ALT_INT = 11;
constexpr std::uint8_t BODY_FRD = 12;
constexpr std::uint8_t RESERVED_13 = 13;
constexpr std::uint8_t RESERVED_14 = 14;
constexpr std::uint8_t RESERVED_15 = 15;
constexpr std::uint8_t RESERVED_16 = 16;
constexpr std::uint8_t RESERVED_17 = 17;
constexpr std::uint8_t RESERVED_18 = 18;
constexpr std::uint8_t RESERVED_19 = 19;
constexpr std::uint8_t LOCAL_FRD = 20;
constexpr std::uint8_t LOCAL_FLU = 21;

}  // namespace MavFrame

}  // namespace mavlink::enumerations
