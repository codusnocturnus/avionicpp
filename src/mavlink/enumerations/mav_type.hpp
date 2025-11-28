#pragma once
#include <cstdint>

namespace mavlink::enumerations {

/// @brief MAV_TYPE
/// @see https://mavlink.io/en/messages/common.html#MAV_TYPE
namespace MavType {
constexpr std::uint8_t GENERIC = 0;                    ///< Generic micro air vehicle
constexpr std::uint8_t FIXED_WING = 1;                 ///< Fixed wing aircraft.
constexpr std::uint8_t QUADROTOR = 2;                  ///< Quadrotor
constexpr std::uint8_t COAXIAL = 3;                    ///< Coaxial helicopter
constexpr std::uint8_t HELICOPTER = 4;                 ///< Normal helicopter with tail rotor.
constexpr std::uint8_t ANTENNA_TRACKER = 5;            ///< Ground installation
constexpr std::uint8_t GCS = 6;                        ///< Operator control unit / ground control station
constexpr std::uint8_t AIRSHIP = 7;                    ///< Airship, controlled
constexpr std::uint8_t FREE_BALLOON = 8;               ///< Free balloon, uncontrolled
constexpr std::uint8_t ROCKET = 9;                     ///< Rocket
constexpr std::uint8_t GROUND_ROVER = 10;              ///< Ground rover
constexpr std::uint8_t SURFACE_BOAT = 11;              ///< Surface vessel, boat, ship
constexpr std::uint8_t SUBMARINE = 12;                 ///< Submarine
constexpr std::uint8_t HEXAROTOR = 13;                 ///< Hexarotor
constexpr std::uint8_t OCTOROTOR = 14;                 ///< Octorotor
constexpr std::uint8_t TRICOPTER = 15;                 ///< Tricopter
constexpr std::uint8_t FLAPPING_WING = 16;             ///< Flapping wing
constexpr std::uint8_t KITE = 17;                      ///< Kite
constexpr std::uint8_t ONBOARD_CONTROLLER = 18;        ///< Onboard companion controller
constexpr std::uint8_t VTOL_TAILSITTER_DUOROTOR = 19;  ///< Two-rotor Tailsitter VTOL that additionally uses control
                                                       ///< surfaces in vertical operation. Note, value previously named
                                                       ///< MAV_TYPE_VTOL_DUOROTOR.
constexpr std::uint8_t VTOL_TAILSITTER_QUADROTOR = 20;  ///< Quad-rotor Tailsitter VTOL using a V-shaped quad config in
                                                        ///< vertical operation. Note: value previously named
                                                        ///< MAV_TYPE_VTOL_QUADROTOR.
constexpr std::uint8_t VTOL_TILTROTOR = 21;   ///< Tiltrotor VTOL. Fuselage and wings stay (nominally) horizontal in all
                                              ///< flight phases. It able  to tilt (some) rotors to provide thrust in
                                              ///< cruise flight.
constexpr std::uint8_t VTOL_FIXEDROTOR = 22;  ///< VTOL with separate fixed rotors for hover and cruise flight. Fuselage
                                              ///< and wings stay (nominally) horizontal in all flight phases.
constexpr std::uint8_t VTOL_TAILSITTER = 23;  ///< Tailsitter VTOL. Fuselage and wings orientation changes depending on
                                              ///< flight phase: vertical for hover, horizontal for cruise. Use more
                                              ///< specific VTOL MAV_TYPE_VTOL_TAILSITTER_DUOROTOR or
                                              ///< MAV_TYPE_VTOL_TAILSITTER_QUADROTOR if appropriate.
constexpr std::uint8_t VTOL_TILTWING = 24;    ///< Tiltwing VTOL. Fuselage stays horizontal in all flight phases. The
                                              ///< whole wing, along with   any attached engine, can tilt between
                                              ///< vertical and horizontal mode.
constexpr std::uint8_t VTOL_RESERVED5 = 25;   ///< VTOL reserved 5
constexpr std::uint8_t GIMBAL = 26;           ///< Gimbal
constexpr std::uint8_t ADSB = 27;             ///< ADSB system
constexpr std::uint8_t PARAFOIL = 28;         ///< Steerable, nonrigid airfoil
constexpr std::uint8_t DODECAROTOR = 29;      ///< Dodecarotor
constexpr std::uint8_t CAMERA = 30;           ///< Camera
constexpr std::uint8_t CHARGING_STATION = 31;  ///< Charging station
constexpr std::uint8_t FLARM = 32;             ///< FLARM collision avoidance system
constexpr std::uint8_t SERVO = 33;             ///< Servo
constexpr std::uint8_t ODID = 34;              ///< Open Drone ID. See https://mavlink.io/en/services/opendroneid.html.
constexpr std::uint8_t DECAROTOR = 35;         ///< Decarotor
constexpr std::uint8_t BATTERY = 36;           ///< Battery
constexpr std::uint8_t PARACHUTE = 37;         ///< Parachute
constexpr std::uint8_t LOG = 38;               ///< Log
constexpr std::uint8_t OSD = 39;               ///< OSD
constexpr std::uint8_t IMU = 40;               ///< IMU
constexpr std::uint8_t GPS = 41;               ///< GPS
constexpr std::uint8_t WINCH = 42;             ///< Winch
constexpr std::uint8_t GENERIC_MULTIROTOR = 43;  ///< Generic multirotor that does not fit into a specific type or whose
                                                 ///< type is unknown
constexpr std::uint8_t ILLUMINATOR = 44;         ///< Illuminator. An illuminator is a light source that is used for
                                                 ///< lighting up dark areas external to the system: e.g. a torch or
                                                 ///< searchlight (as opposed to a light source for illuminating the
                                                 ///< system itself, e.g. an indicator light).
constexpr std::uint8_t SPACECRAFT_ORBITER = 45;  ///< Orbiter spacecraft. Includes satellites orbiting terrestrial and
                                                 ///< extra-terrestrial bodies. Follows NASA Spacecraft Classification.
constexpr std::uint8_t GROUND_QUADRUPED = 46;    ///< A generic four-legged ground vehicle (e.g., a robot dog).
constexpr std::uint8_t VTOL_GYRODYNE = 47;  ///< VTOL hybrid of helicopter and autogyro. It has a main rotor for lift
                                            ///< and separate propellers for forward flight. The rotor must be powered
                                            ///< for hover but can autorotate in cruise flight. See:
                                            ///< https://en.wikipedia.org/wiki/Gyrodyne
constexpr std::uint8_t GRIPPER = 48;        ///< Gripper
constexpr std::uint8_t RADIO = 49;          ///< Radio
}  // namespace MavType

}  // namespace mavlink::enumerations
