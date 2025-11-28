#pragma once
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Time and Date (ZDA).
template <typename Traits>
struct ZDA_T {
    static constexpr std::string_view MessageId = "ZDA"sv;

    /// @brief UTC Time (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief Day (01-31).
    typename Traits::template Int<int, 2> day;

    /// @brief Month (01-12).
    typename Traits::template Int<int, 2> month;

    /// @brief Year (4 digits).
    typename Traits::template Int<int, 4> year;

    /// @brief Local zone description (00 to +/- 13 hours).
    typename Traits::template Int<int, 2> local_zone_hours;

    /// @brief Local zone minutes description (00 to 59).
    typename Traits::template Int<int, 2> local_zone_minutes;
};

using ZDA = ZDA_T<TxTraits>;
using LazyZDA = ZDA_T<RxTraits>;

}  // namespace nmea0183::payloads
