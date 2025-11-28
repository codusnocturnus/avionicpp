#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief GNSS Fix Data (GNS).
template <typename Traits>
struct GNS_T {
    static constexpr std::string_view MessageId = "GNS"sv;

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

    /// @brief Mode Indicator (Variable length string).
    typename Traits::String mode_indicator;

    /// @brief Number of satellites in use.
    typename Traits::template Int<int, 2> num_satellites;

    /// @brief Horizontal Dilution of Precision (HDOP).
    typename Traits::template Float<float, 1> hdop;

    /// @brief Antenna Altitude.
    typename Traits::template Float<float, 1> altitude;

    /// @brief Geoidal separation.
    typename Traits::template Float<float, 1> geoid_separation;

    /// @brief Age of differential data - Optional.
    typename Traits::template Float<float, 1> age_of_differential;

    /// @brief Differential ref station ID - Optional.
    typename Traits::template Int<int, 4> station_id;

    /// @brief Navigational Status Indicator - Optional (NMEA 4.1+).
    typename Traits::template Enum<char> navigational_status;
};

using GNS = GNS_T<TxTraits>;
using LazyGNS = GNS_T<RxTraits>;

}  // namespace nmea0183::payloads
