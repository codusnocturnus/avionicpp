#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/vtg.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("VTG Message Serialization", "[VTG][Serializer]") {
    GIVEN("A VTG message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::VTG> msg;
        msg.payload.course_true.value = 309.62f;
        msg.payload.reference_true.value = nmea0183::enumerations::NorthReference::True;
        msg.payload.course_magnetic.value = 308.5f;
        msg.payload.reference_magnetic.value = nmea0183::enumerations::NorthReference::Magnetic;
        msg.payload.speed_knots.value = 0.13f;
        msg.payload.units_knots.value = nmea0183::enumerations::UnitsIndicator::Knots;
        msg.payload.speed_kph.value = 0.2f;
        msg.payload.units_kph.value = nmea0183::enumerations::UnitsIndicator::KilometersPerHour;
        msg.payload.mode_indicator.value = nmea0183::enumerations::PositionFixModeIndicator::Autonomous;

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPVTG,309.6,T,308.5,M,0.1,N,0.2,K,A*<CS>
                REQUIRE(res.starts_with("$GPVTG,309.6,T,308.5,M,0.1,N,0.2,K,A*"));
            }
        }
    }
}

SCENARIO("VTG Message Deserialization", "[VTG][Deserializer]") {
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

    GIVEN("A view representing a VTG message") {
        auto view = make_view("GP", "VTG", {"309.62", "T", "", "M", "0.13", "N", "0.2", "K", "A"});

        WHEN("Binding to LazyVTG") {
            auto result = bind<payloads::LazyVTG>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& vtg = *result;

                REQUIRE(**vtg.course_true.value() == Catch::Approx(309.62f));
                REQUIRE(**vtg.reference_true.value() == enumerations::NorthReference::True);
                REQUIRE_FALSE(vtg.course_magnetic.value()->has_value());
                REQUIRE(**vtg.reference_magnetic.value() == enumerations::NorthReference::Magnetic);
                REQUIRE(**vtg.speed_knots.value() == Catch::Approx(0.13f));
                REQUIRE(**vtg.units_knots.value() == enumerations::UnitsIndicator::Knots);
                REQUIRE(**vtg.speed_kph.value() == Catch::Approx(0.2f));
                REQUIRE(**vtg.mode_indicator.value() == enumerations::PositionFixModeIndicator::Autonomous);
            }
        }
    }
}
