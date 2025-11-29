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

TEST_CASE("NMEA Utilities: Timestamp RMC", "[nmea0183][utilities]") {
    nmea0183::payloads::RMC rmc;

    SECTION("Valid timestamp") {
        rmc.utc_time.value = 123456.00;  // 12:34:56
        rmc.date.value = 230324;         // 23 March 2024

        auto ts = get_timestamp(rmc);
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

    SECTION("Missing date") {
        rmc.utc_time.value = 123456.00;
        rmc.date.value = std::nullopt;
        CHECK_FALSE(get_timestamp(rmc).has_value());
    }

    SECTION("Missing time") {
        rmc.utc_time.value = std::nullopt;
        rmc.date.value = 230324;
        CHECK_FALSE(get_timestamp(rmc).has_value());
    }

    SECTION("Invalid date (ymd not ok)") {
        rmc.utc_time.value = 123456.00;
        rmc.date.value = 999999;  // Invalid
        CHECK_FALSE(get_timestamp(rmc).has_value());
    }

    SECTION("Set timestamp") {
        auto tp = sys_days{year{2023} / 10 / 15} + hours{10} + minutes{30} + seconds{45};
        set_timestamp(rmc, clock_cast<utc_clock>(tp));

        REQUIRE(rmc.utc_time.value.has_value());
        CHECK_THAT(*rmc.utc_time.value, Catch::Matchers::WithinAbs(103045.0, 0.001));

        REQUIRE(rmc.date.value.has_value());
        CHECK(*rmc.date.value == 151023);
    }
}

TEST_CASE("NMEA Utilities: Timestamp ZDA", "[nmea0183][utilities]") {
    nmea0183::payloads::ZDA zda;

    SECTION("Valid timestamp") {
        zda.utc_time.value = 010203.00;
        zda.day.value = 1;
        zda.month.value = 1;
        zda.year.value = 2025;

        auto ts = get_timestamp(zda);
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

    SECTION("Missing fields") {
        zda.utc_time.value = 123456.00;
        zda.day.value = 1;
        zda.month.value = std::nullopt;  // Missing month
        zda.year.value = 2024;
        CHECK_FALSE(get_timestamp(zda).has_value());
    }

    SECTION("Set timestamp") {
        auto tp = sys_days{year{2022} / 2 / 28} + hours{23} + minutes{59} + seconds{59};
        set_timestamp(zda, clock_cast<utc_clock>(tp));

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

TEST_CASE("NMEA Utilities: Latitude/Longitude", "[nmea0183][utilities]") {
    nmea0183::payloads::RMC p;

    SECTION("Get Latitude") {
        p.latitude.value = 4530.00;  // 45 deg 30 min = 45.5
        p.latitude_direction.value = enumerations::DirectionIndicator::North;
        auto lat = get_latitude_deg(p);
        REQUIRE(lat.has_value());
        CHECK_THAT(*lat, Catch::Matchers::WithinAbs(45.5, 0.0001));

        p.latitude_direction.value = enumerations::DirectionIndicator::South;
        lat = get_latitude_deg(p);
        REQUIRE(lat.has_value());
        CHECK_THAT(*lat, Catch::Matchers::WithinAbs(-45.5, 0.0001));

        p.latitude.value = std::nullopt;
        CHECK_FALSE(get_latitude_deg(p).has_value());
    }

    SECTION("Set Latitude") {
        set_latitude_deg(p, 45.5);
        REQUIRE(p.latitude.value.has_value());
        CHECK_THAT(*p.latitude.value, Catch::Matchers::WithinAbs(4530.0, 0.001));
        REQUIRE(p.latitude_direction.value.has_value());
        CHECK(*p.latitude_direction.value == enumerations::DirectionIndicator::North);

        set_latitude_deg(p, -45.5);
        REQUIRE(p.latitude.value.has_value());
        CHECK_THAT(*p.latitude.value, Catch::Matchers::WithinAbs(4530.0, 0.001));
        REQUIRE(p.latitude_direction.value.has_value());
        CHECK(*p.latitude_direction.value == enumerations::DirectionIndicator::South);
    }

    SECTION("Get Longitude") {
        p.longitude.value = 12030.00;  // 120 deg 30 min = 120.5
        p.longitude_direction.value = enumerations::DirectionIndicator::East;
        auto lon = get_longitude_deg(p);
        REQUIRE(lon.has_value());
        CHECK_THAT(*lon, Catch::Matchers::WithinAbs(120.5, 0.0001));

        p.longitude_direction.value = enumerations::DirectionIndicator::West;
        lon = get_longitude_deg(p);
        REQUIRE(lon.has_value());
        CHECK_THAT(*lon, Catch::Matchers::WithinAbs(-120.5, 0.0001));
    }

    SECTION("Set Longitude") {
        set_longitude_deg(p, 120.5);
        REQUIRE(p.longitude.value.has_value());
        CHECK_THAT(*p.longitude.value, Catch::Matchers::WithinAbs(12030.0, 0.001));
        REQUIRE(p.longitude_direction.value.has_value());
        CHECK(*p.longitude_direction.value == enumerations::DirectionIndicator::East);

        set_longitude_deg(p, -120.5);
        REQUIRE(p.longitude.value.has_value());
        CHECK_THAT(*p.longitude.value, Catch::Matchers::WithinAbs(12030.0, 0.001));
        REQUIRE(p.longitude_direction.value.has_value());
        CHECK(*p.longitude_direction.value == enumerations::DirectionIndicator::West);
    }
}
