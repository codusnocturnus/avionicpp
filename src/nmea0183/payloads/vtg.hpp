#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Course Over Ground and Ground Speed (VTG).
template <typename Traits>
struct VTG_T {
    static constexpr std::string_view MessageId = "VTG"sv;

    /// @brief Course over ground (degrees True).
    typename Traits::template Float<float, 1> course_true;

    /// @brief Reference (T = True).
    typename Traits::template Enum<char> reference_true;

    /// @brief Course over ground (degrees Magnetic).
    typename Traits::template Float<float, 1> course_magnetic;

    /// @brief Reference (M = Magnetic).
    typename Traits::template Enum<char> reference_magnetic;

    /// @brief Speed over ground (knots).
    typename Traits::template Float<float, 1> speed_knots;

    /// @brief Units (N = Knots).
    typename Traits::template Enum<char> units_knots;

    /// @brief Speed over ground (km/hr).
    typename Traits::template Float<float, 1> speed_kph;

    /// @brief Units (K = Kilometers).
    typename Traits::template Enum<char> units_kph;

    /// @brief Mode Indicator (A/D/E/M/S/N) - Optional.
    typename Traits::template Enum<char> mode_indicator;
};

using VTG = VTG_T<TxTraits>;
using LazyVTG = VTG_T<RxTraits>;

}  // namespace nmea0183::payloads
