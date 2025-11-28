#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/hdt.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

// Helpers
uint8_t calculate_checksum_ref_hdt(std::string_view content) {
    uint8_t sum = 0;
    for (char c : content) {
        sum ^= static_cast<uint8_t>(c);
    }
    return sum;
}

SCENARIO("HDT Message Serialization", "[HDT][Serializer]") {
    GIVEN("An HDT message") {
        std::array<char, 100> buffer;

        auto msg =
            nmea0183::Message<"HE", nmea0183::payloads::HDT>{.payload = {.heading = {270.4f}, .true_indicator = {'T'}}};

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);

            THEN("It produces the correct sentence structure") {
                std::string res(buffer.data(), len);

                // Basic checks
                REQUIRE(res.starts_with("$HEHDT,270.4,T*"));
                REQUIRE(res.ends_with("\r\n"));

                // Extract checksum to verify calculation logic matches reference
                auto content = res.substr(1, res.find('*') - 1);  // "HEHDT,270.4,T"
                auto cs_str = res.substr(res.find('*') + 1, 2);

                uint8_t ref_cs = calculate_checksum_ref_hdt(content);
                char expected_hex[3];
                snprintf(expected_hex, 3, "%02X", ref_cs);

                REQUIRE(cs_str == expected_hex);
            }
        }
    }
}

SCENARIO("HDT Message Deserialization", "[HDT][Deserializer]") {
    auto make_view = [](std::string_view talker, std::string_view type, std::initializer_list<std::string_view> args) {
        nmea0183::MessageView view;
        view.talker_id = talker;
        view.message_type = type;
        size_t i = 0;
        for (auto arg : args) {
            if (i < nmea0183::MessageView::MaxFields) {
                view.fields[i++] = arg;
            }
        }
        view.field_count = i;
        return view;
    };

    using TestRegistry = nmea0183::Dispatcher<nmea0183::MessageHandler<"HDT", nmea0183::payloads::LazyHDT>>;

    GIVEN("A populated Registry") {
        WHEN("Dispatching a valid HDT message") {
            auto view = make_view("HE", "HDT", {"270.0", "T"});

            bool handled = TestRegistry::dispatch(
                view, nmea0183::overloaded{[](std::expected<nmea0183::payloads::LazyHDT, nmea0183::NMEAError> msg) {
                                               REQUIRE(msg.has_value());
                                               REQUIRE(**msg->heading.value() == Catch::Approx(270.0f));
                                           },
                                           [](auto) { FAIL("Wrong handler called"); }});

            REQUIRE(handled == true);
        }
    }
}
