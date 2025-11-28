#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Datum Reference message (DTM).
template <typename Traits>
struct DTM_T {
    static constexpr std::string_view MessageId = "DTM"sv;

    /// @brief Local Datum Code (e.g. W84, W72, 999).
    typename Traits::String local_datum_code;

    /// @brief Local Datum Subdivision Code.
    typename Traits::String local_datum_subdivision_code;

    /// @brief Latitude Offset in minutes.
    typename Traits::template Float<double, 4> latitude_offset;

    /// @brief Latitude Offset Direction (N/S).
    typename Traits::template Enum<char> latitude_offset_direction;

    /// @brief Longitude Offset in minutes.
    typename Traits::template Float<double, 4> longitude_offset;

    /// @brief Longitude Offset Direction (E/W).
    typename Traits::template Enum<char> longitude_offset_direction;

    /// @brief Altitude Offset in meters.
    typename Traits::template Float<double, 1> altitude_offset;

    /// @brief Reference Datum Code (e.g. W84).
    typename Traits::String reference_datum_code;
};

using DTM = DTM_T<TxTraits>;
using LazyDTM = DTM_T<RxTraits>;

}  // namespace nmea0183::payloads
