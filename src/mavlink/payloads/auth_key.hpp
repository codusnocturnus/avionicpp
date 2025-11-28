#pragma once
#include <array>
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Auth Key (Msg ID 7).
/// @details Emit an encrypted signature / key identifying this system. PLEASE NOTE: This protocol has been kept simple,
/// so transmitting the key requires an encrypted channel for true safety.
template <typename Traits>
struct AuthKey_T {
    static constexpr std::uint32_t MessageId = 7;
    static constexpr std::string_view MessageName = "AUTH_KEY";

    typename Traits::template Field<std::array<char, 32>> key;  ///< key

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<AuthKey_T<Traits>>();
};

using AuthKey = AuthKey_T<TxTraits>;
using LazyAuthKey = AuthKey_T<RxTraits>;

}  // namespace mavlink::payloads
