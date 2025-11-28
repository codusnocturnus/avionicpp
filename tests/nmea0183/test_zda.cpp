#include <array>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "nmea0183/deserializer.hpp"
#include "nmea0183/payloads/zda.hpp"
#include "nmea0183/serializer.hpp"

using namespace std::string_view_literals;

SCENARIO("ZDA Message Serialization", "[ZDA][Serializer]") {
    GIVEN("A ZDA message with specific date and time") {
        // 12:34:56.78 UTC, 21st Nov 2025, Zone 00:00
        nmea0183::Message<"GP", nmea0183::payloads::ZDA> msg;
        msg.payload.utc_time.value = 123456.78;
        msg.payload.day.value = 21;
        msg.payload.month.value = 11;
        msg.payload.year.value = 2025;
        msg.payload.local_zone_hours.value = 0;
        msg.payload.local_zone_minutes.value = 0;

        std::array<char, 100> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is formatted correctly with zero padding") {
                // Expect: $GPZDA,123456.78,21,11,2025,00,00*<CS>
                REQUIRE(res.starts_with("$GPZDA,123456.78,21,11,2025,00,00*"));
            }
        }
    }

    GIVEN("A ZDA message with single digit date values") {
        // 1st Jan 2025
        nmea0183::Message<"GP", nmea0183::payloads::ZDA> msg;
        msg.payload.utc_time.value = 010203.00;  // 1:02:03
        msg.payload.day.value = 1;
        msg.payload.month.value = 1;
        msg.payload.year.value = 2025;
        msg.payload.local_zone_hours.value = 0;
        msg.payload.local_zone_minutes.value = 0;

        std::array<char, 100> buffer;

        WHEN("Serialized") {
            auto len = nmea0183::serialize(msg, buffer);
            std::string res(buffer.data(), len);

            THEN("It is padded with zeros") {
                // Expect: $GPZDA,010203.00,01,01,2025,00,00*
                REQUIRE(res.starts_with("$GPZDA,010203.00,01,01,2025,00,00*"));
            }
        }
    }
}
