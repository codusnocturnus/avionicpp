#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Local Position NED (Msg ID 32).
/// @details The filtered local position (e.g. fused computer vision and accelerometers). Coordinate frame is
/// right-handed, Z-axis down (aeronautical frame, NED / north-east-down convention)
template <typename Traits>
struct LocalPositionNed_T {
    static constexpr std::uint32_t MessageId = 32;
    static constexpr std::string_view MessageName = "LOCAL_POSITION_NED";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<float> x;                     ///< X Position
    typename Traits::template Field<float> y;                     ///< Y Position
    typename Traits::template Field<float> z;                     ///< Z Position
    typename Traits::template Field<float> vx;                    ///< X Speed
    typename Traits::template Field<float> vy;                    ///< Y Speed
    typename Traits::template Field<float> vz;                    ///< Z Speed

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<LocalPositionNed_T<Traits>>();
};

using LocalPositionNed = LocalPositionNed_T<TxTraits>;
using LazyLocalPositionNed = LocalPositionNed_T<RxTraits>;

}  // namespace mavlink::payloads
