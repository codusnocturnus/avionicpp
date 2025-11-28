#pragma once

namespace nmea0183::enumerations {

/// @brief Indicates the method used to obtain the position fix.
namespace PositionFixModeIndicator {
constexpr char Autonomous = 'A';
constexpr char Differential = 'D';
constexpr char Estimated = 'E';
constexpr char FloatRTK = 'F';
constexpr char ManualInput = 'M';
constexpr char NoFix = 'N';
constexpr char Precise = 'P';
constexpr char RealTimeKinematic = 'R';
constexpr char Simulator = 'S';
}  // namespace PositionFixModeIndicator

/// @brief Cardinal directions for Latitude and Longitude.
namespace DirectionIndicator {
constexpr char East = 'E';
constexpr char North = 'N';
constexpr char South = 'S';
constexpr char West = 'W';
}  // namespace DirectionIndicator

/// @brief Units for various measurements (Speed, Distance).
namespace UnitsIndicator {
constexpr char KilometersPerHour = 'K';
constexpr char Meters = 'M';
constexpr char Knots = 'N';
}  // namespace UnitsIndicator

/// @brief Quality indicator for the GPS fix (from GGA message).
namespace PositionFixQuality {
constexpr char NoFix = '0';
constexpr char Autonomous = '1';
constexpr char Differential = '2';
constexpr char PPSMode = '3';
constexpr char RealTimeKinematic = '4';
constexpr char FloatRTK = '5';
constexpr char Estimated = '6';
constexpr char ManualInput = '7';
constexpr char Simulator = '8';
}  // namespace PositionFixQuality

/// @brief General status indicator (Active/Void).
namespace StatusIndicator {
constexpr char Active = 'A';
constexpr char Void = 'V';
}  // namespace StatusIndicator

/// @brief Mode of operation (Automatic/Manual) for selecting satellites (GSA).
namespace FixMode {
constexpr char Automatic = 'A';
constexpr char Manual = 'M';
}  // namespace FixMode

/// @brief Type of fix (2D, 3D, or None) (GSA).
namespace FixType {
constexpr char NoFix = '1';
constexpr char TwoDimensional = '2';
constexpr char ThreeDimensional = '3';
}  // namespace FixType

/// @brief Reference for heading (True/Magnetic).
namespace NorthReference {
constexpr char Magnetic = 'M';
constexpr char True = 'T';
}  // namespace NorthReference

/// @brief Indicator of safety status.
namespace SafeIndicator {
constexpr char Safe = 'S';
constexpr char Unsafe = 'U';
}  // namespace SafeIndicator

/// @brief Navigational status indicators.
namespace NavigationalStatus {
constexpr char Safe = 'S';
constexpr char Caution = 'C';
constexpr char Unsafe = 'U';
constexpr char NotValid = 'V';
}  // namespace NavigationalStatus

}  // namespace nmea0183::enumerations
