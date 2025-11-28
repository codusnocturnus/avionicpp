#pragma once
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief GNSS Satellite Fault Detection (GBS).
template <typename Traits>
struct GBS_T {
    static constexpr std::string_view MessageId = "GBS"sv;

    /// @brief UTC Time of the GGA or GNS fix associated with this sentence (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief Expected error in latitude (meters).
    typename Traits::template Float<float, 1> lat_error;

    /// @brief Expected error in longitude (meters).
    typename Traits::template Float<float, 1> lon_error;

    /// @brief Expected error in altitude (meters).
    typename Traits::template Float<float, 1> alt_error;

    /// @brief ID number of most likely failed satellite.
    typename Traits::template Int<int, 2> satellite_id;

    /// @brief Probability of missed detection for most likely failed satellite.
    typename Traits::template Float<float, 3> probability;

    /// @brief Estimate of bias in meters on most likely failed satellite.
    typename Traits::template Float<float, 1> bias;

    /// @brief Standard deviation of bias estimate.
    typename Traits::template Float<float, 1> std_dev;
};

using GBS = GBS_T<TxTraits>;
using LazyGBS = GBS_T<RxTraits>;

}  // namespace nmea0183::payloads
