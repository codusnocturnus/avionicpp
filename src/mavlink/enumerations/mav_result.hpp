#pragma once
#include <cstdint>

namespace mavlink::enumerations {

namespace MavResult {
/// @brief Result from a MAVLink command (MAV_CMD)
constexpr std::uint8_t ACCEPTED = 0;
constexpr std::uint8_t TEMPORARILY_REJECTED = 1;
constexpr std::uint8_t DENIED = 2;
constexpr std::uint8_t UNSUPPORTED = 3;
constexpr std::uint8_t FAILED = 4;
constexpr std::uint8_t IN_PROGRESS = 5;
constexpr std::uint8_t CANCELLED = 6;
constexpr std::uint8_t COMMAND_LONG_ONLY = 7;
constexpr std::uint8_t COMMAND_INT_ONLY = 8;
constexpr std::uint8_t COMMAND_UNSUPPORTED_MAV_FRAME = 9;
constexpr std::uint8_t NOT_IN_CONTROL = 10;

}  // namespace MavResult

}  // namespace mavlink::enumerations
