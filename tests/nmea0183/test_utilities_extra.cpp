#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <chrono>
#include <optional>
#include "nmea0183/enumerations.hpp"
#include "nmea0183/payloads/rmc.hpp"
#include "nmea0183/payloads/zda.hpp"
#include "nmea0183/utilities.hpp"

using namespace nmea0183;
using namespace std::chrono;

SCENARIO("NMEA Utilities: Timestamp extraction from RMC", "[nmea0183][utilities]") {
    GIVEN("An RMC payload") {
        nmea0183::payloads::RMC rmc;

        WHEN("The payload has valid date and time") {
            rmc.utc_time.value = 123456.00;  // 12:34:56
            rmc.date.value = 230324;         // 23 March 2024

            auto ts = get_timestamp(rmc);

            THEN("A valid time_point is returned") {
                REQUIRE(ts.has_value());

                auto sys_time = clock_cast<system_clock>(*ts);
                auto dp = floor<days>(sys_time);
                year_month_day ymd{dp};

                CHECK(ymd.year() == year{2024});
                CHECK(ymd.month() == month{3});
                CHECK(ymd.day() == day{23});

                auto time = sys_time - dp;
                auto hms = hh_mm_ss{time};
                CHECK(hms.hours().count() == 12);
                CHECK(hms.minutes().count() == 34);
                CHECK(hms.seconds().count() == 56);
            }
        }

        WHEN("The payload is missing the date") {
            rmc.utc_time.value = 123456.00;
            rmc.date.value = std::nullopt;

            THEN("Timestamp extraction fails") {
                CHECK_FALSE(get_timestamp(rmc).has_value());
            }
        }

        WHEN("The payload is missing the time") {
            rmc.utc_time.value = std::nullopt;
            rmc.date.value = 230324;

            THEN("Timestamp extraction fails") {
                CHECK_FALSE(get_timestamp(rmc).has_value());
            }
        }

        WHEN("The date is invalid") {
            rmc.utc_time.value = 123456.00;
            rmc.date.value = 999999;  // Invalid

            THEN("Timestamp extraction fails") {
                CHECK_FALSE(get_timestamp(rmc).has_value());
            }
        }

        WHEN("Setting the timestamp from a time_point") {
            auto tp = sys_days{year{2023} / 10 / 15} + hours{10} + minutes{30} + seconds{45};
            set_timestamp(rmc, clock_cast<utc_clock>(tp));

            THEN("The payload fields are populated correctly") {
                REQUIRE(rmc.utc_time.value.has_value());
                CHECK_THAT(*rmc.utc_time.value, Catch::Matchers::WithinAbs(103045.0, 0.001));

                REQUIRE(rmc.date.value.has_value());
                CHECK(*rmc.date.value == 151023);
            }
        }
    }
}

SCENARIO("NMEA Utilities: Timestamp extraction from ZDA", "[nmea0183][utilities]") {
    GIVEN("A ZDA payload") {
        nmea0183::payloads::ZDA zda;

        WHEN("The payload has valid date and time") {
            zda.utc_time.value = 010203.00;
            zda.day.value = 1;
            zda.month.value = 1;
            zda.year.value = 2025;

            auto ts = get_timestamp(zda);

            THEN("A valid time_point is returned") {
                REQUIRE(ts.has_value());

                auto sys_time = clock_cast<system_clock>(*ts);
                auto dp = floor<days>(sys_time);
                year_month_day ymd{dp};

                CHECK(ymd.year() == year{2025});
                CHECK(ymd.month() == month{1});
                CHECK(ymd.day() == day{1});

                auto time = sys_time - dp;
                auto hms = hh_mm_ss{time};
                CHECK(hms.hours().count() == 1);
                CHECK(hms.minutes().count() == 2);
                CHECK(hms.seconds().count() == 3);
            }
        }

        WHEN("The payload is missing fields") {
            zda.utc_time.value = 123456.00;
            zda.day.value = 1;
            zda.month.value = std::nullopt;  // Missing month
            zda.year.value = 2024;

            THEN("Timestamp extraction fails") {
                CHECK_FALSE(get_timestamp(zda).has_value());
            }
        }

        WHEN("Setting the timestamp from a time_point") {
            auto tp = sys_days{year{2022} / 2 / 28} + hours{23} + minutes{59} + seconds{59};
            set_timestamp(zda, clock_cast<utc_clock>(tp));

            THEN("The payload fields are populated correctly") {
                REQUIRE(zda.utc_time.value.has_value());
                CHECK_THAT(*zda.utc_time.value, Catch::Matchers::WithinAbs(235959.0, 0.001));

                REQUIRE(zda.day.value.has_value());
                CHECK(*zda.day.value == 28);
                REQUIRE(zda.month.value.has_value());
                CHECK(*zda.month.value == 2);
                REQUIRE(zda.year.value.has_value());
                CHECK(*zda.year.value == 2022);
            }
        }
    }
}

SCENARIO("NMEA Utilities: Coordinate Conversion", "[nmea0183][utilities]") {
    GIVEN("An RMC payload") {
        nmea0183::payloads::RMC p;

        WHEN("Latitude is populated (North)") {
            p.latitude.value = 4530.00;  // 45 deg 30 min = 45.5
            p.latitude_direction.value = enumerations::DirectionIndicator::North;

            THEN("get_latitude_deg returns positive decimal degrees") {
                auto lat = get_latitude_deg(p);
                REQUIRE(lat.has_value());
                CHECK_THAT(*lat, Catch::Matchers::WithinAbs(45.5, 0.0001));
            }
        }

        WHEN("Latitude is populated (South)") {
            p.latitude.value = 4530.00;
            p.latitude_direction.value = enumerations::DirectionIndicator::South;

            THEN("get_latitude_deg returns negative decimal degrees") {
                auto lat = get_latitude_deg(p);
                REQUIRE(lat.has_value());
                CHECK_THAT(*lat, Catch::Matchers::WithinAbs(-45.5, 0.0001));
            }
        }

        WHEN("Latitude is missing") {
            p.latitude.value = std::nullopt;

            THEN("get_latitude_deg returns nullopt") {
                CHECK_FALSE(get_latitude_deg(p).has_value());
            }
        }

        WHEN("Setting latitude (positive/North)") {
            set_latitude_deg(p, 45.5);

            THEN("Payload fields are set correctly") {
                REQUIRE(p.latitude.value.has_value());
                CHECK_THAT(*p.latitude.value, Catch::Matchers::WithinAbs(4530.0, 0.001));
                REQUIRE(p.latitude_direction.value.has_value());
                CHECK(*p.latitude_direction.value == enumerations::DirectionIndicator::North);
            }
        }

        WHEN("Setting latitude (negative/South)") {
            set_latitude_deg(p, -45.5);

            THEN("Payload fields are set correctly") {
                REQUIRE(p.latitude.value.has_value());
                CHECK_THAT(*p.latitude.value, Catch::Matchers::WithinAbs(4530.0, 0.001));
                REQUIRE(p.latitude_direction.value.has_value());
                CHECK(*p.latitude_direction.value == enumerations::DirectionIndicator::South);
            }
        }

        WHEN("Longitude is populated (East)") {
            p.longitude.value = 12030.00;  // 120 deg 30 min = 120.5
            p.longitude_direction.value = enumerations::DirectionIndicator::East;

            THEN("get_longitude_deg returns positive decimal degrees") {
                auto lon = get_longitude_deg(p);
                REQUIRE(lon.has_value());
                CHECK_THAT(*lon, Catch::Matchers::WithinAbs(120.5, 0.0001));
            }
        }

        WHEN("Longitude is populated (West)") {
            p.longitude.value = 12030.00;
            p.longitude_direction.value = enumerations::DirectionIndicator::West;

            THEN("get_longitude_deg returns negative decimal degrees") {
                auto lon = get_longitude_deg(p);
                REQUIRE(lon.has_value());
                CHECK_THAT(*lon, Catch::Matchers::WithinAbs(-120.5, 0.0001));
            }
        }

        WHEN("Setting longitude (positive/East)") {
            set_longitude_deg(p, 120.5);

            THEN("Payload fields are set correctly") {
                REQUIRE(p.longitude.value.has_value());
                CHECK_THAT(*p.longitude.value, Catch::Matchers::WithinAbs(12030.0, 0.001));
                REQUIRE(p.longitude_direction.value.has_value());
                CHECK(*p.longitude_direction.value == enumerations::DirectionIndicator::East);
            }
        }

        WHEN("Setting longitude (negative/West)") {
            set_longitude_deg(p, -120.5);

            THEN("Payload fields are set correctly") {
                REQUIRE(p.longitude.value.has_value());
                CHECK_THAT(*p.longitude.value, Catch::Matchers::WithinAbs(12030.0, 0.001));
                REQUIRE(p.longitude_direction.value.has_value());
                CHECK(*p.longitude_direction.value == enumerations::DirectionIndicator::West);
            }
        }
    }
}
