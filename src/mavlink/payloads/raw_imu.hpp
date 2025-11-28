#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief Raw IMU (Msg ID 27).
/// @details The RAW IMU readings for a 9DOF sensor, which is identified by the id (default IMU1). This message should
/// always contain the true raw values without any scaling to allow data capture and system debugging.
template <typename Traits>
struct RawImu_T {
    static constexpr std::uint32_t MessageId = 27;
    static constexpr std::string_view MessageName = "RAW_IMU";

    typename Traits::template Field<std::uint64_t>
        time_usec;  ///< Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp
                    ///< format (since 1.1.1970 or since system boot) by checking for the magnitude of the number.
    typename Traits::template Field<std::int16_t> xacc;   ///< X acceleration (raw)
    typename Traits::template Field<std::int16_t> yacc;   ///< Y acceleration (raw)
    typename Traits::template Field<std::int16_t> zacc;   ///< Z acceleration (raw)
    typename Traits::template Field<std::int16_t> xgyro;  ///< Angular speed around X axis (raw)
    typename Traits::template Field<std::int16_t> ygyro;  ///< Angular speed around Y axis (raw)
    typename Traits::template Field<std::int16_t> zgyro;  ///< Angular speed around Z axis (raw)
    typename Traits::template Field<std::int16_t> xmag;   ///< X Magnetic field (raw)
    typename Traits::template Field<std::int16_t> ymag;   ///< Y Magnetic field (raw)
    typename Traits::template Field<std::int16_t> zmag;   ///< Z Magnetic field (raw)

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra = calculate_crc_extra<RawImu_T<Traits>>();
};

using RawImu = RawImu_T<TxTraits>;
using LazyRawImu = RawImu_T<RxTraits>;

}  // namespace mavlink::payloads
