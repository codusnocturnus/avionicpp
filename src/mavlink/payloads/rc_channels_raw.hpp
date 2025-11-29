#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief RC Channels Raw (Msg ID 35).
/// @details The RAW values of the RC channels received. The standard PPM modulation is as follows: 1000 microseconds:
/// 0%, 2000 microseconds: 100%. A value of UINT16_MAX implies the channel is unused. Individual receivers/transmitters
/// might violate this specification.
template <typename Traits>
struct RcChannelsRaw_T {
    static constexpr std::uint32_t MessageId = 35;
    static constexpr std::string_view MessageName = "RC_CHANNELS_RAW";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<std::uint16_t> chan1_raw;     ///< RC channel 1 value.
    typename Traits::template Field<std::uint16_t> chan2_raw;     ///< RC channel 2 value.
    typename Traits::template Field<std::uint16_t> chan3_raw;     ///< RC channel 3 value.
    typename Traits::template Field<std::uint16_t> chan4_raw;     ///< RC channel 4 value.
    typename Traits::template Field<std::uint16_t> chan5_raw;     ///< RC channel 5 value.
    typename Traits::template Field<std::uint16_t> chan6_raw;     ///< RC channel 6 value.
    typename Traits::template Field<std::uint16_t> chan7_raw;     ///< RC channel 7 value.
    typename Traits::template Field<std::uint16_t> chan8_raw;     ///< RC channel 8 value.
    typename Traits::template Field<std::uint8_t> port;  ///< Servo output port (set of 8 outputs = 1 port). Flight
                                                         ///< stacks running on Pixhawk should use: 0 = MAIN, 1 = AUX.
    typename Traits::template Field<std::uint8_t> rssi;  ///< Receive signal strength indicator in device-dependent
                                                         ///< units/scale. Values: [0-254], UINT8_MAX: invalid/unknown.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<RcChannelsRaw_T<Traits>>(); }
};

using RcChannelsRaw = RcChannelsRaw_T<TxTraits>;
using LazyRcChannelsRaw = RcChannelsRaw_T<RxTraits>;

}  // namespace mavlink::payloads
