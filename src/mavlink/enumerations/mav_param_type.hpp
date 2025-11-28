#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief Specifies the datatype of a MAVLink parameter.
namespace MavParamType {

constexpr std::uint8_t UINT8 = 1;    ///< 8-bit unsigned integer
constexpr std::uint8_t INT8 = 2;     ///< 8-bit signed integer
constexpr std::uint8_t UINT16 = 3;   ///< 16-bit unsigned integer
constexpr std::uint8_t INT16 = 4;    ///< 16-bit signed integer
constexpr std::uint8_t UINT32 = 5;   ///< 32-bit unsigned integer
constexpr std::uint8_t INT32 = 6;    ///< 32-bit signed integer
constexpr std::uint8_t UINT64 = 7;   ///< 64-bit unsigned integer
constexpr std::uint8_t INT64 = 8;    ///< 64-bit signed integer
constexpr std::uint8_t REAL32 = 9;   ///< 32-bit floating-point
constexpr std::uint8_t REAL64 = 10;  ///< 64-bit floating-point

}  // namespace MavParamType

}  // namespace mavlink::enumerations
