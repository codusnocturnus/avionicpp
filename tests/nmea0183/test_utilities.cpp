#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>

#include "nmea0183/payloads/gll.hpp"
#include "nmea0183/payloads/rmc.hpp"
#include "nmea0183/payloads/zda.hpp"
#include "nmea0183/utilities.hpp"

SCENARIO("Utilities Time Conversion", "[Utilities][Time]") {
    using namespace std::chrono;

    GIVEN("A ZDA payload populated manually") {
        nmea0183::payloads::ZDA payload;
        payload.utc_time.value = 143000.00;
        payload.day.value = 25;
        payload.month.value = 12;
        payload.year.value = 2024;

        WHEN("Converted to time_point") {
            auto tp_opt = nmea0183::get_timestamp(payload);
            THEN("It returns a valid time_point") {
                REQUIRE(tp_opt.has_value());
                auto ymd = year_month_day{year(2024), month(12), day(25)};
                auto sys_days = std::chrono::sys_days{ymd};
                auto expected_tp = sys_days + hours(14) + minutes(30);
                auto expected_utc = clock_cast<utc_clock>(expected_tp);
                auto diff = duration_cast<milliseconds>(*tp_opt - expected_utc).count();
                REQUIRE(std::abs(diff) < 10);
            }
        }
    }

    GIVEN("An RMC payload populated manually") {
        nmea0183::payloads::RMC payload;
        payload.utc_time.value = 102030.00;  // 10:20:30
        payload.date.value = 251224;         // 25 Dec 24 (2024)

        WHEN("Converted to time_point") {
            auto tp_opt = nmea0183::get_timestamp(payload);
            THEN("It returns a valid time_point") {
                REQUIRE(tp_opt.has_value());
                auto ymd = year_month_day{year(2024), month(12), day(25)};
                auto sys_days = std::chrono::sys_days{ymd};
                auto expected_tp = sys_days + hours(10) + minutes(20) + seconds(30);
                auto expected_utc = clock_cast<utc_clock>(expected_tp);
                auto diff = duration_cast<milliseconds>(*tp_opt - expected_utc).count();
                REQUIRE(std::abs(diff) < 10);
            }
        }
    }

    GIVEN("A time_point") {
        auto ymd = year_month_day{year(2023), month(10), day(5)};
        auto sys_days = std::chrono::sys_days{ymd};
        auto tp_sys = sys_days + hours(9) + minutes(15) + seconds(30);
        auto tp_utc = clock_cast<utc_clock>(tp_sys);

        nmea0183::payloads::ZDA payload;

        WHEN("Populating payload from time_point") {
            nmea0183::set_timestamp(payload, tp_utc);

            THEN("Fields are set correctly") {
                REQUIRE(payload.year.value == 2023);
                REQUIRE(payload.month.value == 10);
                REQUIRE(payload.day.value == 5);
                REQUIRE(payload.utc_time.value == Catch::Approx(91530.0));
            }
        }
    }

    GIVEN("A time_point for RMC") {
        auto ymd = year_month_day{year(2023), month(10), day(5)};
        auto sys_days = std::chrono::sys_days{ymd};
        auto tp_sys = sys_days + hours(9) + minutes(15) + seconds(30);
        auto tp_utc = clock_cast<utc_clock>(tp_sys);

        nmea0183::payloads::RMC payload;

        WHEN("Populating payload from time_point") {
            nmea0183::set_timestamp(payload, tp_utc);

            THEN("Fields are set correctly") {
                // 051023
                REQUIRE(payload.date.value == 51023);
                REQUIRE(payload.utc_time.value == Catch::Approx(91530.0));
            }
        }
    }
}

SCENARIO("Utilities Coordinate Conversion", "[Utilities][Coord]") {
    GIVEN("A GLL payload with Latitude") {
        nmea0183::payloads::GLL payload;
        // 48 deg 07.038 min North = 48 + 7.038/60 = 48.1173
        payload.latitude.value = 4807.038;
        payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::North;

        WHEN("Getting latitude in degrees") {
            auto lat = nmea0183::get_latitude_deg(payload);
            THEN("It matches expected decimal degrees") {
                REQUIRE(lat.has_value());
                REQUIRE(*lat == Catch::Approx(48.1173));
            }
        }
    }

    GIVEN("A GLL payload with South Latitude") {
        nmea0183::payloads::GLL payload;
        payload.latitude.value = 4807.038;
        payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::South;

        WHEN("Getting latitude") {
            auto lat = nmea0183::get_latitude_deg(payload);
            THEN("It is negative") {
                REQUIRE(lat.has_value());
                REQUIRE(*lat == Catch::Approx(-48.1173));
            }
        }
    }

    GIVEN("A decimal latitude") {
        double lat_deg = -12.5;  // 12 deg 30 min South
        nmea0183::payloads::GLL payload;

        WHEN("Setting latitude") {
            nmea0183::set_latitude_deg(payload, lat_deg);
            THEN("Payload fields are correct") {
                REQUIRE(payload.latitude.value == Catch::Approx(1230.0));
                REQUIRE(payload.latitude_direction.value == nmea0183::enumerations::DirectionIndicator::South);
            }
        }
    }
}
