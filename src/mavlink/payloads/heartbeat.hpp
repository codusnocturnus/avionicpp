#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_autopilot.hpp"
#include "mavlink/enumerations/mav_mode_flag.hpp"
#include "mavlink/enumerations/mav_state.hpp"
#include "mavlink/enumerations/mav_type.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Heartbeat (Msg ID 0).
/// @details The heartbeat message shows that a system or component is present and responding. The type and autopilot
/// fields (along with the message component id), allow the receiving system to treat further messages from this system
/// appropriately (e.g. by laying out the user interface based on the autopilot). This microservice is documented at
/// https://mavlink.io/en/services/heartbeat.html
template <typename Traits>
struct Heartbeat_T {
    static constexpr std::uint32_t MessageId = 0;
    static constexpr std::string_view MessageName = "HEARTBEAT";

    typename Traits::template Field<std::uint32_t> custom_mode;  ///< A bitfield for use for autopilot-specific flags
    typename Traits::template Field<std::uint8_t>
        type;  ///< Vehicle or component type. For a flight controller component the vehicle type (quadrotor,
               ///< helicopter, etc.). For other components the component type (e.g. camera, gimbal, etc.). This should
               ///< be used in preference to component id for identifying the component type.
    typename Traits::template Field<std::uint8_t> autopilot;  ///< Autopilot type / class. Use MAV_AUTOPILOT_INVALID for
                                                              ///< components that are not flight controllers.
    typename Traits::template Field<std::uint8_t> base_mode;        ///< System mode bitmap.
    typename Traits::template Field<std::uint8_t> system_status;    ///< System status flag.
    typename Traits::template Field<std::uint8_t> mavlink_version;  ///< MAVLink version, not writable by user, gets
                                                                    ///< added by protocol because of magic data type:
                                                                    ///< uint8_t_mavlink_version

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<Heartbeat_T<Traits>>(); }
};

using Heartbeat = Heartbeat_T<TxTraits>;
using LazyHeartbeat = Heartbeat_T<RxTraits>;

}  // namespace mavlink::payloads
