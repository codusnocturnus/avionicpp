#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

/// @brief Heading - True (HDT).
template <typename Traits>
struct HDT_T {
    static constexpr std::string_view MessageId = "HDT"sv;

    /// @brief Heading in degrees.
    typename Traits::template Float<float, 1> heading;
    /// @brief Indicator for True heading (always 'T').
    typename Traits::template Enum<char> true_indicator;
};

using HDT = HDT_T<TxTraits>;
using LazyHDT = HDT_T<RxTraits>;

}  // namespace nmea0183::payloads
