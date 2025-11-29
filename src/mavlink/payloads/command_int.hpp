#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_frame.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Command Int (Msg ID 75).
/// @details Send a command with up to seven parameters to the MAV, where params 5 and 6 are integers and the other
/// values are floats. This is preferred over COMMAND_LONG as it allows the MAV_FRAME to be specified for interpreting
/// positional information, such as altitude.
template <typename Traits>
struct CommandInt_T {
    static constexpr std::uint32_t MessageId = 75;
    static constexpr std::string_view MessageName = "COMMAND_INT";

    typename Traits::template Field<float> param1;                   ///< PARAM1, see MAV_CMD enum
    typename Traits::template Field<float> param2;                   ///< PARAM2, see MAV_CMD enum
    typename Traits::template Field<float> param3;                   ///< PARAM3, see MAV_CMD enum
    typename Traits::template Field<float> param4;                   ///< PARAM4, see MAV_CMD enum
    typename Traits::template Field<std::int32_t> x;                 ///< PARAM5 / local: x position, global: latitude
    typename Traits::template Field<std::int32_t> y;                 ///< PARAM6 / local: y position, global: longitude
    typename Traits::template Field<float> z;                        ///< PARAM7 / z position: global: altitude
    typename Traits::template Field<std::uint16_t> command;          ///< The scheduled action for the mission item.
    typename Traits::template Field<std::uint8_t> target_system;     ///< System ID
    typename Traits::template Field<std::uint8_t> target_component;  ///< Component ID
    typename Traits::template Field<std::uint8_t> frame;             ///< The coordinate system of the COMMAND.
    typename Traits::template Field<std::uint8_t> current;           ///< Not used.
    typename Traits::template Field<std::uint8_t> autocontinue;      ///< Not used (set 0).

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<CommandInt_T<Traits>>(); }
};

using CommandInt = CommandInt_T<TxTraits>;
using LazyCommandInt = CommandInt_T<RxTraits>;

}  // namespace mavlink::payloads
