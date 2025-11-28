#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gll.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GLL Message Serialization", "[GLL][Serializer]") {
    GIVEN("A GLL message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::GLL> msg;
        msg.payload.latitude.value = 4807.038;
        msg.payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::North;
        msg.payload.longitude.value = 01131.000;
        msg.payload.longitude_direction.value = nmea0183::enumerations::DirectionIndicator::East;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.status.value = nmea0183::enumerations::StatusIndicator::Active;
        msg.payload.mode_indicator.value = nmea0183::enumerations::PositionFixModeIndicator::Autonomous;

        std::array<char, 100> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPGLL,4807.0380,N,01131.0000,E,123456.78,A,A*<CS>
                // Lat Width 9, 4 decimals. 4807.038 -> 4807.0380 (9 chars)
                // Lon Width 10, 4 decimals. 1131.0 -> 01131.0000 (10 chars)
                // Time Width 9, 2 decimals.
                REQUIRE(res.starts_with("$GPGLL,4807.0380,N,01131.0000,E,123456.78,A,A*"));
            }
        }
    }
}

SCENARIO("GLL Message Deserialization", "[GLL][Deserializer]") {
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

    GIVEN("A view representing a GLL message") {
        auto view = make_view("GP", "GLL", {"4807.038", "N", "01131.000", "E", "123456", "A", "A"});

        WHEN("Binding to LazyGLL") {
            auto result = bind<payloads::LazyGLL>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gll = *result;

                REQUIRE(**gll.latitude.value() == Catch::Approx(4807.038));
                REQUIRE(**gll.latitude_direction.value() == enumerations::DirectionIndicator::North);
                REQUIRE(**gll.longitude.value() == Catch::Approx(1131.0));
                REQUIRE(**gll.longitude_direction.value() == enumerations::DirectionIndicator::East);
                REQUIRE(**gll.utc_time.value() == Catch::Approx(123456.0));
                REQUIRE(**gll.status.value() == enumerations::StatusIndicator::Active);
                REQUIRE(**gll.mode_indicator.value() == enumerations::PositionFixModeIndicator::Autonomous);
            }
        }
    }
}
