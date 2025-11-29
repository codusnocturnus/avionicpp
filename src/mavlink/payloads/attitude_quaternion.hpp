#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Attitude Quaternion (Msg ID 31).
/// @details The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right), expressed as quaternion.
/// Quaternion order is w, x, y, z and a zero rotation would be expressed as (1 0 0 0).
template <typename Traits>
struct AttitudeQuaternion_T {
    static constexpr std::uint32_t MessageId = 31;
    static constexpr std::string_view MessageName = "ATTITUDE_QUATERNION";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<float> q1;                    ///< Quaternion component 1, w (1 in null-rotation)
    typename Traits::template Field<float> q2;                    ///< Quaternion component 2, x (0 in null-rotation)
    typename Traits::template Field<float> q3;                    ///< Quaternion component 3, y (0 in null-rotation)
    typename Traits::template Field<float> q4;                    ///< Quaternion component 4, z (0 in null-rotation)
    typename Traits::template Field<float> rollspeed;             ///< Roll angular speed
    typename Traits::template Field<float> pitchspeed;            ///< Pitch angular speed
    typename Traits::template Field<float> yawspeed;              ///< Yaw angular speed

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<AttitudeQuaternion_T<Traits>>(); }
};

using AttitudeQuaternion = AttitudeQuaternion_T<TxTraits>;
using LazyAttitudeQuaternion = AttitudeQuaternion_T<RxTraits>;

}  // namespace mavlink::payloads
