#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Geographic Position - Latitude/Longitude (GLL).
template <typename Traits>
struct GLL_T {
    static constexpr std::string_view MessageId = "GLL"sv;

    /// @brief Latitude (ddmm.mm).
    typename Traits::template Float<double, 4, 9> latitude;

    /// @brief Latitude Direction (N/S).
    typename Traits::template Enum<char> latitude_direction;

    /// @brief Longitude (dddmm.mm).
    typename Traits::template Float<double, 4, 10> longitude;

    /// @brief Longitude Direction (E/W).
    typename Traits::template Enum<char> longitude_direction;

    /// @brief UTC Time (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief Status (A=Valid, V=Invalid).
    typename Traits::template Enum<char> status;

    /// @brief Mode Indicator (A=Autonomous, D=Differential, etc.).
    /// @note Optional in older NMEA, but we define it. If missing in Rx, it will be nullopt.
    typename Traits::template Enum<char> mode_indicator;
};

using GLL = GLL_T<TxTraits>;
using LazyGLL = GLL_T<RxTraits>;

}  // namespace nmea0183::payloads
