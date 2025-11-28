#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/gst.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("GST Message Serialization", "[GST][Serializer]") {
    GIVEN("A GST message with values") {
        nmea0183::Message<"GP", nmea0183::payloads::GST> msg;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.rms_std_dev.value = 1.1f;
        msg.payload.semi_major_std_dev.value = 2.2f;
        msg.payload.semi_minor_std_dev.value = 3.3f;
        msg.payload.semi_major_orientation.value = 45.0f;
        msg.payload.latitude_error_std_dev.value = 0.5f;
        msg.payload.longitude_error_std_dev.value = 0.6f;
        msg.payload.altitude_error_std_dev.value = 0.7f;

        std::array<char, 128> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly") {
                // Expected: $GPGST,123456.78,1.1,2.2,3.3,45.0,0.5,0.6,0.7*<CS>
                REQUIRE(res.starts_with("$GPGST,123456.78,1.1,2.2,3.3,45.0,0.5,0.6,0.7*"));
            }
        }
    }
}

SCENARIO("GST Message Deserialization", "[GST][Deserializer]") {
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

    GIVEN("A view representing a GST message") {
        auto view = make_view("GP", "GST", {"123456.78", "1.1", "2.2", "3.3", "45.0", "0.5", "0.6", "0.7"});

        WHEN("Binding to LazyGST") {
            auto result = bind<payloads::LazyGST>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());
                auto& gst = *result;

                REQUIRE(**gst.utc_time.value() == Catch::Approx(123456.78));
                REQUIRE(**gst.rms_std_dev.value() == Catch::Approx(1.1f));
                REQUIRE(**gst.semi_major_std_dev.value() == Catch::Approx(2.2f));
                REQUIRE(**gst.semi_minor_std_dev.value() == Catch::Approx(3.3f));
                REQUIRE(**gst.semi_major_orientation.value() == Catch::Approx(45.0f));
                REQUIRE(**gst.latitude_error_std_dev.value() == Catch::Approx(0.5f));
                REQUIRE(**gst.longitude_error_std_dev.value() == Catch::Approx(0.6f));
                REQUIRE(**gst.altitude_error_std_dev.value() == Catch::Approx(0.7f));
            }
        }
    }
}
