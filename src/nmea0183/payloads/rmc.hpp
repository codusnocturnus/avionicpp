#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Recommended Minimum Navigation Information (RMC).
template <typename Traits>
struct RMC_T {
    static constexpr std::string_view MessageId = "RMC"sv;

    /// @brief UTC Time (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief Status (A=Active, V=Void).
    typename Traits::template Enum<char> status;

    /// @brief Latitude (ddmm.mm).
    typename Traits::template Float<double, 4, 9> latitude;

    /// @brief Latitude Direction (N/S).
    typename Traits::template Enum<char> latitude_direction;

    /// @brief Longitude (dddmm.mm).
    typename Traits::template Float<double, 4, 10> longitude;

    /// @brief Longitude Direction (E/W).
    typename Traits::template Enum<char> longitude_direction;

    /// @brief Speed over ground (knots).
    typename Traits::template Float<float, 1> speed;

    /// @brief Course over ground (degrees true).
    typename Traits::template Float<float, 1> course;

    /// @brief Date (ddmmyy).
    typename Traits::template Int<int, 6> date;

    /// @brief Magnetic Variation (degrees).
    typename Traits::template Float<float, 1> magnetic_variation;

    /// @brief Magnetic Variation Direction (E/W).
    typename Traits::template Enum<char> magnetic_variation_direction;

    /// @brief Mode Indicator (A/D/E/M/S/N) - Optional.
    typename Traits::template Enum<char> mode_indicator;
};

using RMC = RMC_T<TxTraits>;
using LazyRMC = RMC_T<RxTraits>;

}  // namespace nmea0183::payloads
