#pragma once
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief GNSS Pseudorange Error Statistics (GST).
template <typename Traits>
struct GST_T {
    static constexpr std::string_view MessageId = "GST"sv;

    /// @brief UTC Time (hhmmss.ss).
    typename Traits::template Float<double, 2, 9> utc_time;

    /// @brief RMS value of the standard deviation of the range inputs.
    typename Traits::template Float<float, 1> rms_std_dev;

    /// @brief Standard deviation of semi-major axis of error ellipse (meters).
    typename Traits::template Float<float, 1> semi_major_std_dev;

    /// @brief Standard deviation of semi-minor axis of error ellipse (meters).
    typename Traits::template Float<float, 1> semi_minor_std_dev;

    /// @brief Orientation of semi-major axis of error ellipse (degrees from true north).
    typename Traits::template Float<float, 1> semi_major_orientation;

    /// @brief Standard deviation of latitude error (meters).
    typename Traits::template Float<float, 1> latitude_error_std_dev;

    /// @brief Standard deviation of longitude error (meters).
    typename Traits::template Float<float, 1> longitude_error_std_dev;

    /// @brief Standard deviation of altitude error (meters).
    typename Traits::template Float<float, 1> altitude_error_std_dev;
};

using GST = GST_T<TxTraits>;
using LazyGST = GST_T<RxTraits>;

}  // namespace nmea0183::payloads
