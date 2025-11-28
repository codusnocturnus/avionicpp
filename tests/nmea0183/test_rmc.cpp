#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/rmc.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("RMC Message Serialization", "[RMC][Serializer]") {
    GIVEN("An RMC message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::RMC> msg;
        msg.payload.utc_time.value = 123519.00;
        msg.payload.status.value = nmea0183::enumerations::StatusIndicator::Active;
        msg.payload.latitude.value = 4807.038;
        msg.payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::North;
        msg.payload.longitude.value = 01131.000;
        msg.payload.longitude_direction.value = nmea0183::enumerations::DirectionIndicator::East;
        msg.payload.speed.value = 022.4f;
        msg.payload.course.value = 084.4f;
        msg.payload.date.value = 230394;
        msg.payload.magnetic_variation.value = 003.1f;
        msg.payload.magnetic_variation_direction.value = nmea0183::enumerations::DirectionIndicator::West;
        msg.payload.mode_indicator.value = nmea0183::enumerations::PositionFixModeIndicator::Autonomous;

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPRMC,123519.00,A,4807.0380,N,01131.0000,E,22.4,84.4,230394,3.1,W,A*<CS>
                // Note: Speed/Course P=1. 22.4, 84.4.
                // Date: 230394 (width 6).
                // MagVar: 3.1.
                REQUIRE(res.starts_with("$GPRMC,123519.00,A,4807.0380,N,01131.0000,E,22.4,84.4,230394,3.1,W,A*"));
            }
        }
    }
}

SCENARIO("RMC Message Deserialization", "[RMC][Deserializer]") {
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

    GIVEN("A view representing an RMC message") {
        auto view = make_view(
            "GP", "RMC",
            {"123519", "A", "4807.038", "N", "01131.000", "E", "022.4", "084.4", "230394", "003.1", "W", "A"});

        WHEN("Binding to LazyRMC") {
            auto result = bind<payloads::LazyRMC>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& rmc = *result;

                REQUIRE(**rmc.utc_time.value() == Catch::Approx(123519.0));
                REQUIRE(**rmc.status.value() == enumerations::StatusIndicator::Active);
                REQUIRE(**rmc.latitude.value() == Catch::Approx(4807.038));
                REQUIRE(**rmc.latitude_direction.value() == enumerations::DirectionIndicator::North);
                REQUIRE(**rmc.longitude.value() == Catch::Approx(1131.0));
                REQUIRE(**rmc.longitude_direction.value() == enumerations::DirectionIndicator::East);
                REQUIRE(**rmc.speed.value() == Catch::Approx(22.4f));
                REQUIRE(**rmc.course.value() == Catch::Approx(84.4f));
                REQUIRE(**rmc.date.value() == 230394);
                REQUIRE(**rmc.magnetic_variation.value() == Catch::Approx(3.1f));
                REQUIRE(**rmc.magnetic_variation_direction.value() == enumerations::DirectionIndicator::West);
                REQUIRE(**rmc.mode_indicator.value() == enumerations::PositionFixModeIndicator::Autonomous);
            }
        }
    }
}
