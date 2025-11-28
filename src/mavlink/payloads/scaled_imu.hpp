#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Scaled IMU (Msg ID 26).
/// @details The RAW IMU readings for the usual 9DOF sensor setup. This message should contain the scaled values to the
/// described units.
template <typename Traits>
struct ScaledImu_T {
    static constexpr std::uint32_t MessageId = 26;
    static constexpr std::string_view MessageName = "SCALED_IMU";

    typename Traits::template Field<std::uint32_t> time_boot_ms;  ///< Timestamp (time since system boot).
    typename Traits::template Field<std::int16_t> xacc;           ///< X acceleration
    typename Traits::template Field<std::int16_t> yacc;           ///< Y acceleration
    typename Traits::template Field<std::int16_t> zacc;           ///< Z acceleration
    typename Traits::template Field<std::int16_t> xgyro;          ///< Angular speed around X axis
    typename Traits::template Field<std::int16_t> ygyro;          ///< Angular speed around Y axis
    typename Traits::template Field<std::int16_t> zgyro;          ///< Angular speed around Z axis
    typename Traits::template Field<std::int16_t> xmag;           ///< X Magnetic field
    typename Traits::template Field<std::int16_t> ymag;           ///< Y Magnetic field
    typename Traits::template Field<std::int16_t> zmag;           ///< Z Magnetic field

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<ScaledImu_T<Traits>>();
};

using ScaledImu = ScaledImu_T<TxTraits>;
using LazyScaledImu = ScaledImu_T<RxTraits>;

}  // namespace mavlink::payloads
