#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief Bitmask of (optional) autopilot capabilities (64 bit). If a bit is set, the autopilot supports this
/// capability.
namespace MavProtocolCapability {

constexpr std::uint64_t MISSION_FLOAT = 1;  ///< Autopilot supports the MISSION_ITEM float message type.
constexpr std::uint64_t PARAM_FLOAT = 2;    ///< Autopilot supports the new param float message type.
constexpr std::uint64_t MISSION_INT = 4;    ///< Autopilot supports MISSION_ITEM_INT scaled integer message type.
constexpr std::uint64_t COMMAND_INT = 8;    ///< Autopilot supports COMMAND_INT scaled integer message type.
constexpr std::uint64_t PARAM_ENCODE_BYTEWISE = 16;            ///< Parameter protocol uses byte-wise encoding of
                                                               ///< parameter values into param_value (float) fields.
constexpr std::uint64_t FTP = 32;                              ///< Autopilot supports the File Transfer Protocol v1.
constexpr std::uint64_t SET_ATTITUDE_TARGET = 64;              ///< Autopilot supports commanding attitude offboard.
constexpr std::uint64_t SET_POSITION_TARGET_LOCAL_NED = 128;   ///< Autopilot supports commanding position and
                                                               ///< velocity targets in local NED frame.
constexpr std::uint64_t SET_POSITION_TARGET_GLOBAL_INT = 256;  ///< Autopilot supports commanding position and
                                                               ///< velocity targets in global scaled integers.
constexpr std::uint64_t TERRAIN = 512;                         ///< Autopilot supports terrain protocol / data handling.
constexpr std::uint64_t RESERVED3 = 1024;                      ///< Reserved for future use.
constexpr std::uint64_t FLIGHT_TERMINATION = 2048;             ///< Autopilot supports the MAV_CMD_DO_FLIGHTTERMINATION
                                                               ///< command (flight termination).
constexpr std::uint64_t COMPASS_CALIBRATION = 4096;            ///< Autopilot supports onboard compass calibration.
constexpr std::uint64_t MAVLINK2 = 8192;                       ///< Autopilot supports MAVLink version 2.
constexpr std::uint64_t MISSION_FENCE = 16384;                 ///< Autopilot supports mission fence protocol.
constexpr std::uint64_t MISSION_RALLY = 32768;                 ///< Autopilot supports mission rally point protocol.
constexpr std::uint64_t RESERVED2 = 65536;                     ///< Reserved for future use.
constexpr std::uint64_t PARAM_ENCODE_C_CAST = 131072;          ///< Parameter protocol uses C-cast of parameter values
                                                               ///< to set the param_value (float) fields.
constexpr std::uint64_t COMPONENT_IMPLEMENTS_GIMBAL_MANAGER = 262144;  ///< This component implements/is a gimbal
                                                                       ///< manager.

}  // namespace MavProtocolCapability

}  // namespace mavlink::enumerations
