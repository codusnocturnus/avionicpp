#pragma once
#include <cstdint>
#include <string_view>

#include "mavlink/checksum.hpp"
#include "mavlink/types.hpp"

namespace mavlink::payloads {

/// @brief System Time (Msg ID 2).
/// @details The system time is the time of the master clock, typically the computer clock of the main onboard computer.
template <typename Traits>
struct SystemTime_T {
    static constexpr std::uint32_t MessageId = 2;
    static constexpr std::string_view MessageName = "SYSTEM_TIME";

    typename Traits::template Field<std::uint64_t> time_unix_usec;  ///< Timestamp (UNIX epoch time).
    typename Traits::template Field<std::uint32_t> time_boot_ms;    ///< Timestamp (time since system boot).

    // Calculated CRC Extra
    static constexpr std::uint8_t CrcExtra() { return calculate_crc_extra<SystemTime_T<Traits>>(); }
};

using SystemTime = SystemTime_T<TxTraits>;
using LazySystemTime = SystemTime_T<RxTraits>;

}  // namespace mavlink::payloads
