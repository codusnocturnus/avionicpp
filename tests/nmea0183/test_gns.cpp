#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gns.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GNS Message Serialization", "[GNS][Serializer]") {
    GIVEN("A GNS message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::GNS> msg;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.latitude.value = 4807.038;
        msg.payload.latitude_direction.value = nmea0183::enumerations::DirectionIndicator::North;
        msg.payload.longitude.value = 01131.000;
        msg.payload.longitude_direction.value = nmea0183::enumerations::DirectionIndicator::East;
        msg.payload.mode_indicator.value = "AA";
        msg.payload.num_satellites.value = 10;
        msg.payload.hdop.value = 0.9f;
        msg.payload.altitude.value = 545.4f;
        msg.payload.geoid_separation.value = 46.9f;
        msg.payload.age_of_differential.value = std::nullopt;
        msg.payload.station_id.value = std::nullopt;
        msg.payload.navigational_status.value = std::nullopt;  // Older NMEA

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPGNS,123456.78,4807.0380,N,01131.0000,E,AA,10,0.9,545.4,46.9,,,*<CS>
                REQUIRE(res.starts_with("$GPGNS,123456.78,4807.0380,N,01131.0000,E,AA,10,0.9,545.4,46.9,,,*"));
            }
        }

        WHEN("Serialized with Nav Status") {
            msg.payload.navigational_status.value = nmea0183::enumerations::NavigationalStatus::Safe;
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It includes Nav Status") {
                // $GPGNS,...,46.9,,,S*
                REQUIRE(res.starts_with("$GPGNS,123456.78,4807.0380,N,01131.0000,E,AA,10,0.9,545.4,46.9,,,S*"));
            }
        }
    }
}

SCENARIO("GNS Message Deserialization", "[GNS][Deserializer]") {
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

    GIVEN("A view representing a GNS message") {
        auto view =
            make_view("GP", "GNS",
                      {"123456", "4807.038", "N", "01131.000", "E", "AA", "10", "0.9", "545.4", "46.9", "", "", "S"});

        WHEN("Binding to LazyGNS") {
            auto result = bind<payloads::LazyGNS>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gns = *result;

                REQUIRE(**gns.utc_time.value() == Catch::Approx(123456.0));
                REQUIRE(std::string(**gns.mode_indicator.value()) == "AA");
                REQUIRE(**gns.num_satellites.value() == 10);
                REQUIRE(**gns.navigational_status.value() == enumerations::NavigationalStatus::Safe);
            }
        }
    }
}
