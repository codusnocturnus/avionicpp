#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief MAV_STATE
/// @see https://mavlink.io/en/messages/common.html#MAV_STATE
namespace MavState {
constexpr std::uint8_t UNINIT = 0;       ///< Uninitialized system, state is unknown.
constexpr std::uint8_t BOOT = 1;         ///< System is booting up.
constexpr std::uint8_t CALIBRATING = 2;  ///< System is calibrating and not flight-ready.
constexpr std::uint8_t STANDBY = 3;      ///< System is grounded and on standby. It can be launched any time.
constexpr std::uint8_t ACTIVE = 4;       ///< System is active and might be already airborne. Motors are engaged.
constexpr std::uint8_t CRITICAL = 5;     ///< System is in a non-normal flight mode (failsafe). It can however
                                         ///< still navigate.
constexpr std::uint8_t EMERGENCY = 6;  ///< System is in a non-normal flight mode (failsafe). It lost control over parts
                                       ///< or over the whole airframe. It is in mayday and going down.
constexpr std::uint8_t POWEROFF = 7;   ///< System just initialized its power-down sequence, will shut down now.
constexpr std::uint8_t FLIGHT_TERMINATION = 8;  ///< System is terminating itself (failsafe or commanded).
}  // namespace MavState

}  // namespace mavlink::enumerations
