#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief RC Channels Scaled (Msg ID 34).
/// @details The scaled values of the RC channels received: (-100%) -10000, (0%) 0, (100%) 10000. Channels that are
/// inactive should be set to INT16_MAX.
template <typename Traits>
struct RcChannelsScaled_T {
    static constexpr std::uint32_t MessageId = 34;
    static constexpr std::string_view MessageName = "RC_CHANNELS_SCALED";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<std::int16_t> chan1_scaled;   ///< RC channel 1 value scaled.
    typename Traits::template Field<std::int16_t> chan2_scaled;   ///< RC channel 2 value scaled.
    typename Traits::template Field<std::int16_t> chan3_scaled;   ///< RC channel 3 value scaled.
    typename Traits::template Field<std::int16_t> chan4_scaled;   ///< RC channel 4 value scaled.
    typename Traits::template Field<std::int16_t> chan5_scaled;   ///< RC channel 5 value scaled.
    typename Traits::template Field<std::int16_t> chan6_scaled;   ///< RC channel 6 value scaled.
    typename Traits::template Field<std::int16_t> chan7_scaled;   ///< RC channel 7 value scaled.
    typename Traits::template Field<std::int16_t> chan8_scaled;   ///< RC channel 8 value scaled.
    typename Traits::template Field<std::uint8_t> port;  ///< Servo output port (set of 8 outputs = 1 port). Flight
                                                         ///< stacks running on Pixhawk should use: 0 = MAIN, 1 = AUX.
    typename Traits::template Field<std::uint8_t> rssi;  ///< Receive signal strength indicator in device-dependent
                                                         ///< units/scale. Values: [0-254], UINT8_MAX: invalid/unknown.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<RcChannelsScaled_T<Traits>>(); }
};

using RcChannelsScaled = RcChannelsScaled_T<TxTraits>;
using LazyRcChannelsScaled = RcChannelsScaled_T<RxTraits>;

}  // namespace mavlink::payloads
