#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/dtm.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("DTM Message Serialization", "[DTM][Serializer]") {
    GIVEN("A DTM message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::DTM> msg;
        msg.payload.local_datum_code.value = "W84";
        msg.payload.local_datum_subdivision_code.value = "A";
        msg.payload.latitude_offset.value = 0.0025;
        msg.payload.latitude_offset_direction.value = nmea0183::enumerations::DirectionIndicator::South;
        msg.payload.longitude_offset.value = 0.0012;
        msg.payload.longitude_offset_direction.value = nmea0183::enumerations::DirectionIndicator::West;
        msg.payload.altitude_offset.value = -2.5;
        msg.payload.reference_datum_code.value = "W84";

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPDTM,W84,A,0.0025,S,0.0012,W,-2.5,W84*<CS>
                REQUIRE(res.starts_with("$GPDTM,W84,A,0.0025,S,0.0012,W,-2.5,W84*"));
            }
        }
    }
}

SCENARIO("DTM Message Deserialization", "[DTM][Deserializer]") {
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

    GIVEN("A view representing a DTM message") {
        auto view = make_view("GP", "DTM", {"W84", "A", "0.0025", "S", "0.0012", "W", "-2.5", "W84"});

        WHEN("Binding to LazyDTM") {
            auto result = bind<payloads::LazyDTM>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& dtm = *result;

                REQUIRE(std::string(**dtm.local_datum_code.value()) == "W84");
                REQUIRE(std::string(**dtm.local_datum_subdivision_code.value()) == "A");
                REQUIRE(**dtm.latitude_offset.value() == Catch::Approx(0.0025));
                REQUIRE(**dtm.latitude_offset_direction.value() == enumerations::DirectionIndicator::South);
                REQUIRE(**dtm.longitude_offset.value() == Catch::Approx(0.0012));
                REQUIRE(**dtm.longitude_offset_direction.value() == enumerations::DirectionIndicator::West);
                REQUIRE(**dtm.altitude_offset.value() == Catch::Approx(-2.5));
                REQUIRE(std::string(**dtm.reference_datum_code.value()) == "W84");
            }
        }
    }
}
