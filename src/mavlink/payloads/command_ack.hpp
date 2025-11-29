#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/enumerations/mav_cmd.hpp"
#include "mavlink/enumerations/mav_result.hpp"
#include "mavlink/types.hpp"

using namespace mavlink::enumerations;

namespace mavlink::payloads {

/// @brief Command Ack (Msg ID 77).
/// @details Report status of a command. Includes feedback whether the command was executed. The command microservice is
/// documented at https://mavlink.io/en/services/command.html
template <typename Traits>
struct CommandAck_T {
    static constexpr std::uint32_t MessageId = 77;
    static constexpr std::string_view MessageName = "COMMAND_ACK";

    typename Traits::template Field<std::uint16_t> command;  ///< Command ID (of acknowledged command).
    typename Traits::template Field<std::uint8_t> result;    ///< Result of command.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<CommandAck_T<Traits>>(); }
};

using CommandAck = CommandAck_T<TxTraits>;
using LazyCommandAck = CommandAck_T<RxTraits>;

}  // namespace mavlink::payloads
