#pragma once

#include <chrono>
#include <cmath>
#include <optional>

#include "enumerations.hpp"

namespace nmea0183 {

// --- Time Utilities ---

/// @brief Extracts a UTC timestamp from a payload containing time/date fields.
/// @tparam Payload The message payload type (e.g., RMC, ZDA).
/// @param[in] p The payload instance.
/// @return std::optional containing the time_point if successful, or nullopt if fields are missing/invalid.
template <typename Payload>
auto get_timestamp(const Payload& p) -> std::optional<std::chrono::utc_clock::time_point> {
    using namespace std::chrono;

    if (!p.utc_time.value)
        return std::nullopt;

    auto year_val = 0;
    auto month_val = 0u;
    auto day_val = 0u;

    if constexpr (requires { p.date; }) {
        if (!p.date.value)
            return std::nullopt;
        auto ddmmyy = *p.date.value;
        day_val = static_cast<unsigned>(ddmmyy / 10000);
        month_val = static_cast<unsigned>((ddmmyy / 100) % 100);
        year_val = (ddmmyy % 100) + 2000;
    } else if constexpr (requires {
                             p.day;
                             p.month;
                             p.year;
                         }) {
        if (!p.day.value || !p.month.value || !p.year.value)
            return std::nullopt;
        day_val = static_cast<unsigned>(*p.day.value);
        month_val = static_cast<unsigned>(*p.month.value);
        year_val = *p.year.value;
    } else {
        return std::nullopt;
    }

    auto nmea_time = *p.utc_time.value;
    auto hours = static_cast<int>(nmea_time / 10000);
    auto minutes = static_cast<int>(std::fmod(nmea_time, 10000.0) / 100);
    auto seconds_dbl = std::fmod(nmea_time, 100.0);

    auto ymd = year_month_day{std::chrono::year(year_val), std::chrono::month(month_val), std::chrono::day(day_val)};
    if (!ymd.ok())
        return std::nullopt;

    auto sys_days = std::chrono::sys_days{ymd};
    auto time_duration =
        std::chrono::hours(hours) + std::chrono::minutes(minutes) + std::chrono::duration<double>(seconds_dbl);

    auto tp_sys = sys_days + std::chrono::duration_cast<std::chrono::system_clock::duration>(time_duration);
    return std::chrono::clock_cast<std::chrono::utc_clock>(tp_sys);
}

/// @brief Sets the time/date fields in a payload from a UTC timestamp.
/// @tparam Payload The message payload type.
/// @param[in,out] p The payload instance to modify.
/// @param[in] tp The UTC time point.
template <typename Payload>
void set_timestamp(Payload& p, std::chrono::utc_clock::time_point tp) {
    using namespace std::chrono;

    auto sys_tp = clock_cast<system_clock>(tp);
    auto dp = floor<days>(sys_tp);
    auto ymd = year_month_day{dp};

    auto time_of_day = sys_tp - dp;
    auto hh_mm_ss = std::chrono::hh_mm_ss(time_of_day);

    auto seconds = static_cast<double>(hh_mm_ss.seconds().count()) + hh_mm_ss.subseconds().count();
    auto nmea_time = hh_mm_ss.hours().count() * 10000.0 + hh_mm_ss.minutes().count() * 100.0 + seconds;

    p.utc_time.value = nmea_time;

    if constexpr (requires { p.date; }) {
        auto val = static_cast<unsigned>(ymd.day()) * 10000 + static_cast<unsigned>(ymd.month()) * 100 +
                   (static_cast<int>(ymd.year()) % 100);
        p.date.value = val;
    } else if constexpr (requires {
                             p.day;
                             p.month;
                             p.year;
                         }) {
        p.day.value = static_cast<unsigned>(ymd.day());
        p.month.value = static_cast<unsigned>(ymd.month());
        p.year.value = static_cast<int>(ymd.year());
    }
}

// --- Coordinate Utilities ---

namespace detail {
using namespace enumerations;

inline double to_decimal(double nmea_val, char dir) {
    auto degrees = static_cast<int>(nmea_val / 100);
    auto minutes = std::fmod(nmea_val, 100.0);
    auto decimal = degrees + (minutes / 60.0);
    if (dir == DirectionIndicator::South || dir == DirectionIndicator::West) {
        decimal = -decimal;
    }
    return decimal;
}

inline std::pair<double, char> from_decimal_lat(double decimal) {
    auto dir = (decimal >= 0) ? DirectionIndicator::North : DirectionIndicator::South;
    auto abs_val = std::abs(decimal);
    auto degrees = static_cast<int>(abs_val);
    auto minutes = (abs_val - degrees) * 60.0;
    return {degrees * 100.0 + minutes, dir};
}

inline std::pair<double, char> from_decimal_lon(double decimal) {
    auto dir = (decimal >= 0) ? DirectionIndicator::East : DirectionIndicator::West;
    auto abs_val = std::abs(decimal);
    auto degrees = static_cast<int>(abs_val);
    auto minutes = (abs_val - degrees) * 60.0;
    return {degrees * 100.0 + minutes, dir};
}
}  // namespace detail

/// @brief Extracts latitude in decimal degrees from a payload.
/// @tparam Payload The message payload type.
/// @param[in] p The payload instance.
/// @return std::optional containing latitude (positive N, negative S) or nullopt.
template <typename Payload>
auto get_latitude_deg(const Payload& p) -> std::optional<double> {
    if (!p.latitude.value || !p.latitude_direction.value)
        return std::nullopt;
    return detail::to_decimal(*p.latitude.value, *p.latitude_direction.value);
}

/// @brief Sets latitude fields in a payload from decimal degrees.
/// @tparam Payload The message payload type.
/// @param[in,out] p The payload instance to modify.
/// @param[in] latitude Latitude in decimal degrees (positive N, negative S).
template <typename Payload>
void set_latitude_deg(Payload& p, double latitude) {
    auto [val, dir] = detail::from_decimal_lat(latitude);
    p.latitude.value = val;
    p.latitude_direction.value = dir;
}

/// @brief Extracts longitude in decimal degrees from a payload.
/// @tparam Payload The message payload type.
/// @param[in] p The payload instance.
/// @return std::optional containing longitude (positive E, negative W) or nullopt.
template <typename Payload>
auto get_longitude_deg(const Payload& p) -> std::optional<double> {
    if (!p.longitude.value || !p.longitude_direction.value)
        return std::nullopt;
    return detail::to_decimal(*p.longitude.value, *p.longitude_direction.value);
}

/// @brief Sets longitude fields in a payload from decimal degrees.
/// @tparam Payload The message payload type.
/// @param[in,out] p The payload instance to modify.
/// @param[in] longitude Longitude in decimal degrees (positive E, negative W).
template <typename Payload>
void set_longitude_deg(Payload& p, double longitude) {
    auto [val, dir] = detail::from_decimal_lon(longitude);
    p.longitude.value = val;
    p.longitude_direction.value = dir;
}

}  // namespace nmea0183
