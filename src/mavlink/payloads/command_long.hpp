#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Command Long (Msg ID 76).
/// @details Send a command with up to seven parameters to the MAV. COMMAND_INT is generally preferred when sending
/// MAV_CMD commands that include positional information; it offers higher precision and allows the MAV_FRAME to be
/// specified (which may otherwise be ambiguous, particularly for altitude).
template <typename Traits>
struct CommandLong_T {
    static constexpr std::uint32_t MessageId = 76;
    static constexpr std::string_view MessageName = "COMMAND_LONG";

    typename Traits::template Field<float> param1;                ///< Parameter 1 (for the specific command).
    typename Traits::template Field<float> param2;                ///< Parameter 2 (for the specific command).
    typename Traits::template Field<float> param3;                ///< Parameter 3 (for the specific command).
    typename Traits::template Field<float> param4;                ///< Parameter 4 (for the specific command).
    typename Traits::template Field<float> param5;                ///< Parameter 5 (for the specific command).
    typename Traits::template Field<float> param6;                ///< Parameter 6 (for the specific command).
    typename Traits::template Field<float> param7;                ///< Parameter 7 (for the specific command).
    typename Traits::template Field<std::uint16_t> command;       ///< Command ID (of command to send).
    typename Traits::template Field<std::uint8_t> target_system;  ///< System which should execute the command
    typename Traits::template Field<std::uint8_t>
        target_component;  ///< Component which should execute the command, 0 for all components
    typename Traits::template Field<std::uint8_t> confirmation;  ///< 0: First transmission of this command. 1-255:
                                                                 ///< Confirmation transmissions (e.g. for kill command)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<CommandLong_T<Traits>>();
};

using CommandLong = CommandLong_T<TxTraits>;
using LazyCommandLong = CommandLong_T<RxTraits>;

}  // namespace mavlink::payloads
