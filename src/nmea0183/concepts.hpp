#pragma once
#include <type_traits>

namespace nmea0183 {

/// @brief Concept for aggregate types (structs with public members, no constructors).
template <typename T>
concept Aggregate = std::is_aggregate_v<T>;

/// @brief Concept for scoped enumerations (enum class).
template <typename T>
concept ScopedEnumeration = requires { std::is_scoped_enum_v<T>; };

/// @brief Concept to detect if a type is a valid NMEA field wrapper (TxField or RxField).
template <typename T>
concept NmeaField = requires { typename T::ValueType; };

}  // namespace nmea0183
