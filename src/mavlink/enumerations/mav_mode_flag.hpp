#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief MAV_MODE_FLAG
/// @see https://mavlink.io/en/messages/common.html#MAV_MODE_FLAG
namespace MavModeFlag {
constexpr std::uint8_t CUSTOM_MODE_ENABLED = 1;  ///< 0b00000001 system-specific custom mode is enabled. When using this
                                                 ///< flag to enable a custom mode all other flags should be ignored.
constexpr std::uint8_t TEST_ENABLED = 2;  ///< 0b00000010 system has a test mode enabled. This flag is intended for
                                          ///< temporary system tests and should not be used for stable implementations.
constexpr std::uint8_t AUTO_ENABLED = 4;  ///< 0b00000100 autonomous mode enabled, system finds its own goal positions.
                                          ///< Guided flag can be set or not, depends on the actual implementation.
constexpr std::uint8_t GUIDED_ENABLED = 8;  ///< 0b00001000 guided mode enabled, system flies waypoints / mission items.
constexpr std::uint8_t STABILIZE_ENABLED = 16;  ///< 0b00010000 system stabilizes electronically its attitude (and
                                                ///< optionally position). It needs however further control inputs to
                                                ///< move around.
constexpr std::uint8_t HIL_ENABLED = 32;  ///< 0b00100000 hardware in the loop simulation. All motors / actuators are
                                          ///< blocked, but internal software is full operational.
constexpr std::uint8_t MANUAL_INPUT_ENABLED = 64;  ///< 0b01000000 remote control input is enabled.
constexpr std::uint8_t SAFETY_ARMED = 128;  ///< 0b10000000 MAV safety set to armed. Motors are enabled / running / can
                                            ///< start. Ready to fly. Additional note: this flag is to be ignore when
                                            ///< sent in the command MAV_CMD_DO_SET_MODE and
                                            ///< MAV_CMD_COMPONENT_ARM_DISARM shall be used instead. The flag can still
                                            ///< be used to report the armed state.
}  // namespace MavModeFlag

}  // namespace mavlink::enumerations
