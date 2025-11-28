#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gsa.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GSA Message Serialization", "[GSA][Serializer]") {
    GIVEN("A GSA message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::GSA> msg;
        msg.payload.selection_mode.value = nmea0183::enumerations::FixMode::Automatic;
        msg.payload.fix_mode.value = nmea0183::enumerations::FixType::ThreeDimensional;
        msg.payload.sv_id_01.value = 1;
        msg.payload.sv_id_02.value = 2;
        // others null
        msg.payload.pdop.value = 1.5f;
        msg.payload.hdop.value = 1.0f;
        msg.payload.vdop.value = 0.8f;

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPGSA,A,3,01,02,,,,,,,,,,,1.5,1.0,0.8*<CS>
                // sv_id width 2 -> "01", "02".
                // Empty sv_ids are just commas.
                REQUIRE(res.starts_with("$GPGSA,A,3,01,02,,,,,,,,,,,1.5,1.0,0.8*"));
            }
        }
    }
}

SCENARIO("GSA Message Deserialization", "[GSA][Deserializer]") {
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

    GIVEN("A view representing a GSA message") {
        auto view =
            make_view("GP", "GSA", {"A", "3", "01", "02", "", "", "", "", "", "", "", "", "", "", "1.5", "1.0", "0.8"});

        WHEN("Binding to LazyGSA") {
            auto result = bind<payloads::LazyGSA>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gsa = *result;

                REQUIRE(**gsa.selection_mode.value() == enumerations::FixMode::Automatic);
                REQUIRE(**gsa.fix_mode.value() == enumerations::FixType::ThreeDimensional);
                REQUIRE(**gsa.sv_id_01.value() == 1);
                REQUIRE(**gsa.sv_id_02.value() == 2);
                REQUIRE_FALSE(gsa.sv_id_03.value()->has_value());
                REQUIRE(**gsa.pdop.value() == Catch::Approx(1.5f));
                REQUIRE(**gsa.hdop.value() == Catch::Approx(1.0f));
                REQUIRE(**gsa.vdop.value() == Catch::Approx(0.8f));
            }
        }
    }
}
