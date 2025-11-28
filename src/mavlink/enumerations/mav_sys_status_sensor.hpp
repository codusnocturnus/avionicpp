#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief These encode the sensors whose status is sent as part of the SYS_STATUS message.
namespace MavSysStatusSensor {

constexpr std::uint32_t GYRO_3D = 1;                     ///< 0x01 3D gyro
constexpr std::uint32_t ACCEL_3D = 2;                    ///< 0x02 3D accelerometer
constexpr std::uint32_t MAG_3D = 4;                      ///< 0x04 3D magnetometer
constexpr std::uint32_t ABSOLUTE_PRESSURE = 8;           ///< 0x08 absolute pressure
constexpr std::uint32_t DIFFERENTIAL_PRESSURE = 16;      ///< 0x10 differential pressure
constexpr std::uint32_t GPS = 32;                        ///< 0x20 GPS
constexpr std::uint32_t OPTICAL_FLOW = 64;               ///< 0x40 optical flow
constexpr std::uint32_t VISION_POSITION = 128;           ///< 0x80 computer vision position
constexpr std::uint32_t LASER_POSITION = 256;            ///< 0x100 laser based position
constexpr std::uint32_t EXTERNAL_GROUND_TRUTH = 512;     ///< 0x200 external ground truth (Vicon or Leica)
constexpr std::uint32_t ANGULAR_RATE_CONTROL = 1024;     ///< 0x400 3D angular rate control
constexpr std::uint32_t ATTITUDE_STABILIZATION = 2048;   ///< 0x800 attitude stabilization
constexpr std::uint32_t YAW_POSITION = 4096;             ///< 0x1000 yaw position
constexpr std::uint32_t Z_ALTITUDE_CONTROL = 8192;       ///< 0x2000 z-altitude control
constexpr std::uint32_t XY_POSITION_CONTROL = 16384;     ///< 0x4000 x/y position control
constexpr std::uint32_t MOTOR_OUTPUTS = 32768;           ///< 0x8000 motor outputs / control
constexpr std::uint32_t RC_RECEIVER = 65536;             ///< 0x10000 rc receiver
constexpr std::uint32_t GYRO2_3D = 131072;               ///< 0x20000 2nd 3D gyro
constexpr std::uint32_t ACCEL2_3D = 262144;              ///< 0x40000 2nd 3D accelerometer
constexpr std::uint32_t MAG2_3D = 524288;                ///< 0x80000 2nd 3D magnetometer
constexpr std::uint32_t GEOFENCE = 1048576;              ///< 0x100000 geofence
constexpr std::uint32_t AHRS = 2097152;                  ///< 0x200000 AHRS subsystem health
constexpr std::uint32_t TERRAIN = 4194304;               ///< 0x400000 Terrain subsystem health
constexpr std::uint32_t REVERSE_MOTOR = 8388608;         ///< 0x800000 Motors are reversed
constexpr std::uint32_t LOGGING = 16777216;              ///< 0x1000000 Logging
constexpr std::uint32_t BATTERY = 33554432;              ///< 0x2000000 Battery
constexpr std::uint32_t PROXIMITY = 67108864;            ///< 0x4000000 Proximity
constexpr std::uint32_t SATCOM = 134217728;              ///< 0x8000000 Satellite Communication
constexpr std::uint32_t PREARM_CHECK = 268435456;        ///< 0x10000000 Pre-arm check status.
                                                         ///< 0 = not ready or disabled, 1 = ready
constexpr std::uint32_t OBSTACLE_AVOIDANCE = 536870912;  ///< 0x20000000 Obstacle avoidance
constexpr std::uint32_t PROPULSION = 1073741824;         ///< 0x40000000 Propulsion (ESCs, Motors)
constexpr std::uint32_t EXTENSION_USED = 2147483648;     ///< 0x80000000 Extended bit-mask are used.

}  // namespace MavSysStatusSensor

}  // namespace mavlink::enumerations
