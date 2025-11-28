#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Change Operator Control Ack (Msg ID 6).
/// @details Accept / deny control of this MAV
template <typename Traits>
struct ChangeOperatorControlAck_T {
    static constexpr std::uint32_t MessageId = 6;
    static constexpr std::string_view MessageName = "CHANGE_OPERATOR_CONTROL_ACK";

    typename Traits::template Field<std::uint8_t> gcs_system_id;  ///< ID of the GCS this message
    typename Traits::template Field<std::uint8_t>
        control_request;  ///< 0: request control of this MAV, 1: Release control of this MAV
    typename Traits::template Field<std::uint8_t> ack;  ///< 0: ACK, 1: NACK: Wrong passkey, 2: NACK: Unsupported
                                                        ///< passkey encryption method, 3: NACK: Already under control

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<ChangeOperatorControlAck_T<Traits>>();
};

using ChangeOperatorControlAck = ChangeOperatorControlAck_T<TxTraits>;
using LazyChangeOperatorControlAck = ChangeOperatorControlAck_T<RxTraits>;

}  // namespace mavlink::payloads
