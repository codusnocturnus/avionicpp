#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Change Operator Control (Msg ID 5).
/// @details Request to control this MAV
template <typename Traits>
struct ChangeOperatorControl_T {
    static constexpr std::uint32_t MessageId = 5;
    static constexpr std::string_view MessageName = "CHANGE_OPERATOR_CONTROL";

    typename Traits::template Field<std::uint8_t> target_system;  ///< System the GCS requests control for
    typename Traits::template Field<std::uint8_t>
        control_request;  ///< 0: request control of this MAV, 1: Release control of this MAV
    typename Traits::template Field<std::uint8_t>
        version;  ///< 0: key as plaintext, 1-255: future, different hashing/encryption variants. The GCS should in
                  ///< general use the safest mode possible initially and then gradually move down the encryption level
                  ///< if it gets a NACK message indicating an encryption mismatch.
    typename Traits::template Field<std::array<char, 25>>
        passkey;  ///< Password / Key, depending on version plaintext or encrypted. 25 or less characters, NULL
                  ///< terminated. The characters may involve A-Z, a-z, 0-9, and "!?,.-"

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<ChangeOperatorControl_T<Traits>>();
};

using ChangeOperatorControl = ChangeOperatorControl_T<TxTraits>;
using LazyChangeOperatorControl = ChangeOperatorControl_T<RxTraits>;

}  // namespace mavlink::payloads
