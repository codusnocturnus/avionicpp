#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_sys_status_sensor.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief System Status (Msg ID 1).
/// @details The general system state. If the system is following the MAVLink standard, the system state is mainly
/// defined by three orthogonal states/modes: The system mode, which is either LOCKED (motors shut down and locked),
/// MANUAL (system under RC control), GUIDED (system with autonomous position control, position setpoint controlled
/// manually) or AUTO (system guided by path/waypoint planner). The NAV_MODE defined the current flight mode: LIFTOFF
/// (often an open-loop maneuver), LANDING, WAYPOINTS or VECTOR. This represents the internal navigation state machine.
/// The system status shows whether the system is currently active or not and if an emergency occurred. During the
/// CRITICAL and EMERGENCY states the MAV is still considered to be active, but should start emergency procedures
/// autonomously. After a failure occurred it should first state a reason for the failure (one of the internal error
/// codes) and then switch to emergency landing.
template <typename Traits>
struct SysStatus_T {
    static constexpr std::uint32_t MessageId = 1;
    static constexpr std::string_view MessageName = "SYS_STATUS";

    typename Traits::template Field<std::uint32_t>
        onboard_control_sensors_present;  ///< Bitmask showing which onboard controllers and sensors are present.
    typename Traits::template Field<std::uint32_t>
        onboard_control_sensors_enabled;  ///< Bitmask showing which onboard controllers and sensors are enabled.
    typename Traits::template Field<std::uint32_t>
        onboard_control_sensors_health;  ///< Bitmask showing which onboard controllers and sensors are operational or
                                         ///< have an error.
    typename Traits::template Field<std::uint16_t>
        load;  ///< Maximum usage in percent of the mainloop time. Values: [0-1000] - should be multiplied by 0.1 to get
               ///< percentage (1000 = 100%).
    typename Traits::template Field<std::uint16_t> voltage_battery;  ///< Battery voltage, 1000 = 1V.
    typename Traits::template Field<std::int16_t> current_battery;   ///< Battery current, 10 = 10mA.
    typename Traits::template Field<std::uint16_t>
        drop_rate_comm;  ///< Communication drop rate, (UART, I2C, SPI, CAN), dropped packets on all links (packets that
                         ///< were corrupted on reception on the MAV).
    typename Traits::template Field<std::uint16_t>
        errors_comm;  ///< Communication errors (UART, I2C, SPI, CAN), dropped packets on all links (packets that were
                      ///< corrupted on reception on the MAV).
    typename Traits::template Field<std::uint16_t> errors_count1;  ///< Autopilot-specific errors.
    typename Traits::template Field<std::uint16_t> errors_count2;  ///< Autopilot-specific errors.
    typename Traits::template Field<std::uint16_t> errors_count3;  ///< Autopilot-specific errors.
    typename Traits::template Field<std::uint16_t> errors_count4;  ///< Autopilot-specific errors.
    typename Traits::template Field<std::int8_t>
        battery_remaining;  ///< Battery energy remaining, in percent. Values: [0-100], -1: autopilot does not measure
                            ///< the remaining battery.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<SysStatus_T<Traits>>();
};

using SysStatus = SysStatus_T<TxTraits>;
using LazySysStatus = SysStatus_T<RxTraits>;

}  // namespace mavlink::payloads
