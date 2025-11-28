#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gga.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GGA Message Serialization", "[GGA][Serializer]") {
    GIVEN("A GGA message with full values") {
        nmea0183::Message<"GP", nmea0183::payloads::GGA> msg;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.latitude.value = 4807.038;
        msg.payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::North;
        msg.payload.longitude.value = 01131.000;
        msg.payload.longitude_direction.value = nmea0183::enumerations::DirectionIndicator::East;
        msg.payload.quality.value = nmea0183::enumerations::PositionFixQuality::Autonomous;  // 1
        msg.payload.num_satellites.value = 8;
        msg.payload.hdop.value = 0.9f;
        msg.payload.altitude.value = 545.4f;
        msg.payload.altitude_units.value = nmea0183::enumerations::UnitsIndicator::Meters;
        msg.payload.geoid_separation.value = 46.9f;
        msg.payload.geoid_separation_units.value = nmea0183::enumerations::UnitsIndicator::Meters;
        msg.payload.age_of_differential.value = std::nullopt;
        msg.payload.station_id.value = std::nullopt;

        std::array<char, 128> buffer;

        WHEN("Serialized without DGPS") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // $GPGGA,123456.78,4807.0380,N,01131.0000,E,1,08,0.9,545.4,M,46.9,M,,*<CS>
                // Note: Satellites 08 (width 2). Quality 1 (enum value 1).
                // Empty DGPS fields at end.
                REQUIRE(res.starts_with("$GPGGA,123456.78,4807.0380,N,01131.0000,E,1,08,0.9,545.4,M,46.9,M,,*"));
            }
        }

        WHEN("Serialized with DGPS") {
            msg.payload.quality.value = nmea0183::enumerations::PositionFixQuality::Differential;  // 2
            msg.payload.age_of_differential.value = 1.5f;
            msg.payload.station_id.value = 1023;

            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It includes DGPS fields") {
                // $GPGGA,123456.78,4807.0380,N,01131.0000,E,2,08,0.9,545.4,M,46.9,M,1.5,1023*
                REQUIRE(res.starts_with("$GPGGA,123456.78,4807.0380,N,01131.0000,E,2,08,0.9,545.4,M,46.9,M,1.5,1023*"));
            }
        }
    }
}

SCENARIO("GGA Message Deserialization", "[GGA][Deserializer]") {
    using namespace nmea0183;
    auto make_view = [](std::string_view talker, std::string_view type, std::initializer_list<std::string_view> args) {
        MessageView view;
        view.talker_id = talker;
        view.message_type = type;
        size_t i = 0;
        for (auto arg : args) {
            if (i < MessageView::MaxFields) {
                view.fields[i++] = arg;
            }
        }
        view.field_count = i;
        return view;
    };

    GIVEN("A view representing a GGA message") {
        auto view = make_view(
            "GP", "GGA",
            {"123456", "4807.038", "N", "01131.000", "E", "1", "08", "0.9", "545.4", "M", "46.9", "M", "", ""});

        WHEN("Binding to LazyGGA") {
            auto result = bind<payloads::LazyGGA>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gga = *result;

                REQUIRE(**gga.utc_time.value() == Catch::Approx(123456.0));
                REQUIRE(**gga.latitude.value() == Catch::Approx(4807.038));
                REQUIRE(**gga.num_satellites.value() == 8);
                REQUIRE(**gga.altitude.value() == Catch::Approx(545.4f));
                REQUIRE_FALSE(gga.age_of_differential.value()->has_value());
            }
        }
    }
}
