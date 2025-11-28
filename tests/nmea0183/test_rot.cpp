#include <array>
#include <string_view>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/rot.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

// Helpers
uint8_t calculate_checksum_ref_rot(std::string_view content) {
    uint8_t sum = 0;
    for (char c : content) {
        sum ^= static_cast<uint8_t>(c);
    }
    return sum;
}

void REQUIRE_NMEA_MATCH_ROT(const std::array<char, 100>& buffer, size_t len, std::string expected) {
    std::string result(buffer.data(), len);
    REQUIRE(result == expected);
}

SCENARIO("ROT Message Serialization", "[ROT][Serializer]") {
    GIVEN("A buffer and an ROT message structure") {
        std::array<char, 100> buffer;

        // Initialize with TxTraits (the default alias 'ROT')
        auto msg = nmea0183::Message<"GP", nmea0183::payloads::ROT>{
            .payload = {.rate_of_turn = {35.5f}, .status = {nmea0183::enumerations::StatusIndicator::Active}}};

        WHEN("Serialized with positive values") {
            auto len = serialize(msg, buffer);

            THEN("The output follows NMEA 0183 standard") {
                REQUIRE_NMEA_MATCH_ROT(buffer, len, "$GPROT,35.5,A*02\r\n");
            }
        }

        WHEN("Serialized with negative values") {
            msg.payload.rate_of_turn.value = -10.2f;
            msg.payload.status.value = nmea0183::enumerations::StatusIndicator::Void;

            auto len = serialize(msg, buffer);

            THEN("The negative sign is preserved") {
                REQUIRE_NMEA_MATCH_ROT(buffer, len, "$GPROT,-10.2,V*38\r\n");
            }
        }
    }
}

SCENARIO("ROT Message Deserialization", "[ROT][Deserializer]") {
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

    GIVEN("A view representing a standard ROT message") {
        auto view = make_view("GP", "ROT", {"35.5", "A"});

        WHEN("Binding to LazyROT") {
            auto result = nmea0183::bind<nmea0183::payloads::LazyROT>(view);

            THEN("The bind succeeds") {
                REQUIRE(result.has_value());

                nmea0183::payloads::LazyROT& rot = *result;
                REQUIRE(**rot.rate_of_turn.value() == Catch::Approx(35.5f));
                REQUIRE(**rot.status.value() == nmea0183::enumerations::StatusIndicator::Active);
            }
        }
    }
}
