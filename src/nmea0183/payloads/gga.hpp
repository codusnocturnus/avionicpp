#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Global Positioning System Fix Data (GGA).
template <typename Traits>
struct GGA_T {
    static constexpr std::string_view MessageId = "GGA"sv;

    /// @brief UTC Time (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief Latitude (ddmm.mm).
    typename Traits::template Float<double, 4, 9> latitude;

    /// @brief Latitude Direction (N/S).
    typename Traits::template Enum<char> latitude_direction;

    /// @brief Longitude (dddmm.mm).
    typename Traits::template Float<double, 4, 10> longitude;

    /// @brief Longitude Direction (E/W).
    typename Traits::template Enum<char> longitude_direction;

    /// @brief GPS Quality Indicator.
    typename Traits::template Enum<char> quality;

    /// @brief Number of satellites in use.
    typename Traits::template Int<int, 2> num_satellites;

    /// @brief Horizontal Dilution of Precision (HDOP).
    typename Traits::template Float<float, 1> hdop;

    /// @brief Antenna Altitude above/below mean sea level (geoid).
    typename Traits::template Float<float, 1> altitude;

    /// @brief Units of antenna altitude (M).
    typename Traits::template Enum<char> altitude_units;

    /// @brief Geoidal separation.
    typename Traits::template Float<float, 1> geoid_separation;

    /// @brief Units of geoidal separation (M).
    typename Traits::template Enum<char> geoid_separation_units;

    /// @brief Age of Differential GPS data (seconds) - Optional.
    typename Traits::template Float<float, 1> age_of_differential;

    /// @brief Differential reference station ID (0000-1023) - Optional.
    typename Traits::template Int<int, 4> station_id;
};

using GGA = GGA_T<TxTraits>;
using LazyGGA = GGA_T<RxTraits>;

}  // namespace nmea0183::payloads
