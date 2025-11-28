#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief MAV_AUTOPILOT
/// @see https://mavlink.io/en/messages/common.html#MAV_AUTOPILOT
namespace MavAutopilot {
constexpr std::uint8_t GENERIC = 0;        ///< Generic autopilot, full support for everything
constexpr std::uint8_t RESERVED = 1;       ///< Reserved for future use.
constexpr std::uint8_t SLUGS = 2;          ///< SLUGS autopilot, http://slugsuav.soe.ucsc.edu
constexpr std::uint8_t ARDUPILOTMEGA = 3;  ///< ArduPilot - Plane/Copter/Rover/Sub/Tracker, https://ardupilot.org
constexpr std::uint8_t OPENPILOT = 4;      ///< OpenPilot, http://openpilot.org
constexpr std::uint8_t GENERIC_WAYPOINTS_ONLY = 5;  ///< Generic autopilot only supporting simple waypoints
constexpr std::uint8_t GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY = 6;  ///< Generic autopilot supporting
                                                                          ///< waypoints and other simple
                                                                          ///< navigation commands
constexpr std::uint8_t GENERIC_MISSION_FULL = 7;  ///< Generic autopilot supporting the full mission command set
constexpr std::uint8_t INVALID = 8;               ///< No valid autopilot, e.g. a GCS or other MAVLink component
constexpr std::uint8_t PPZ = 9;                   ///< PPZ UAV - http://nongnu.org/paparazzi
constexpr std::uint8_t UDB = 10;                  ///< UAV Dev Board
constexpr std::uint8_t FP = 11;                   ///< FlexiPilot
constexpr std::uint8_t PX4 = 12;                  ///< PX4 Autopilot - http://px4.io/
constexpr std::uint8_t SMACCMPILOT = 13;          ///< SMACCMPilot - http://smaccmpilot.org
constexpr std::uint8_t AUTOQUAD = 14;             ///< AutoQuad -- http://autoquad.org
constexpr std::uint8_t ARMAZILA = 15;             ///< Armazila -- http://armazila.com
constexpr std::uint8_t AEROB = 16;                ///< Aerob -- http://aerob.ru
constexpr std::uint8_t ASLUAV = 17;               ///< ASLUAV autopilot -- http://www.asl.ethz.ch
constexpr std::uint8_t SMARTAP = 18;              ///< SmartAP Autopilot - http://sky-drones.com
constexpr std::uint8_t AIRRAILS = 19;             ///< AirRails - http://uaventure.com
constexpr std::uint8_t REFLEX = 20;               ///< Fusion Reflex - https://fusion.engineering
}  // namespace MavAutopilot

}  // namespace mavlink::enumerations
