#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Attitude (Msg ID 30).
/// @details The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right).
template <typename Traits>
struct Attitude_T {
    static constexpr std::uint32_t MessageId = 30;
    static constexpr std::string_view MessageName = "ATTITUDE";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<float> roll;                  ///< Roll angle (-pi..+pi).
    typename Traits::template Field<float> pitch;                 ///< Pitch angle (-pi..+pi).
    typename Traits::template Field<float> yaw;                   ///< Yaw angle (-pi..+pi).
    typename Traits::template Field<float> rollspeed;             ///< Roll angular speed.
    typename Traits::template Field<float> pitchspeed;            ///< Pitch angular speed.
    typename Traits::template Field<float> yawspeed;              ///< Yaw angular speed.

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<Attitude_T<Traits>>(); }
};

using Attitude = Attitude_T<TxTraits>;
using LazyAttitude = Attitude_T<RxTraits>;

}  // namespace mavlink::payloads
