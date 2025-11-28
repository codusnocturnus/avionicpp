#pragma once
#include "nmea0183/enumerations.hpp"
#include "nmea0183/types.hpp"

namespace nmea0183::payloads {

// Generic definition: Logic is same for Tx and Rx, storage differs.

/// @brief Rate of Turn (ROT).
template <typename Traits>
struct ROT_T {
    static constexpr std::string_view MessageId = "ROT"sv;

    /// @brief Rate of turn, degrees per minute, "-" means bow turns to port.
    typename Traits::template Float<float, 1> rate_of_turn;

    /// @brief Status (A=Valid, V=Invalid).
    typename Traits::template Enum<char> status;
};

// Concrete Types for users
using ROT = ROT_T<TxTraits>;      // For Serialize
using LazyROT = ROT_T<RxTraits>;  // For Deserialize

}  // namespace nmea0183::payloads
