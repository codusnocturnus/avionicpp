#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gbs.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GBS Message Serialization", "[GBS][Serializer]") {
    GIVEN("A GBS message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::GBS> msg;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.lat_error.value = 1.2f;
        msg.payload.lon_error.value = 3.4f;
        msg.payload.alt_error.value = 5.6f;
        msg.payload.satellite_id.value = 15;
        msg.payload.probability.value = 0.001f;
        msg.payload.bias.value = -2.3f;
        msg.payload.std_dev.value = 0.5f;

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Time width 9: 123456.78 -> 123456.78 (length 9). No padding needed if matches.
                // LatErr: 1.2
                // SatID: 15 (width 2)
                // Prob: 0.001 (precision 3)
                // Bias: -2.3
                // Std: 0.5
                // Expected string base: "$GPGBS,123456.78,1.2,3.4,5.6,15,0.001,-2.3,0.5*"
                REQUIRE(res.starts_with("$GPGBS,123456.78,1.2,3.4,5.6,15,0.001,-2.3,0.5*"));
            }
        }
    }
}

SCENARIO("GBS Message Deserialization", "[GBS][Deserializer]") {
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

    GIVEN("A view representing a GBS message") {
        auto view = make_view("GP", "GBS", {"123456.78", "1.1", "2.2", "3.3", "05", "0.05", "-1.0", "0.2"});

        WHEN("Binding to LazyGBS") {
            auto result = bind<payloads::LazyGBS>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gbs = *result;

                REQUIRE(**gbs.utc_time.value() == Catch::Approx(123456.78));
                REQUIRE(**gbs.lat_error.value() == Catch::Approx(1.1f));
                REQUIRE(**gbs.lon_error.value() == Catch::Approx(2.2f));
                REQUIRE(**gbs.alt_error.value() == Catch::Approx(3.3f));
                REQUIRE(**gbs.satellite_id.value() == 5);
                REQUIRE(**gbs.probability.value() == Catch::Approx(0.05f));
                REQUIRE(**gbs.bias.value() == Catch::Approx(-1.0f));
                REQUIRE(**gbs.std_dev.value() == Catch::Approx(0.2f));
            }
        }
    }
}
