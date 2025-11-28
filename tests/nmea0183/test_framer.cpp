#include <array>
#include <expected>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include "nmea0183/framer.hpp"
#include "nmea0183/types.hpp"

using namespace std::string_literals;

// Helper function to push a string into the framer
auto push_string(nmea0183::Framer& framer, std::string_view sv) -> nmea0183::Framer::YieldedValue {
    for (char c : sv) {
        auto yielded = framer.push_byte(c);
        if (yielded) {
            const auto& result = *yielded;
            return result;
        }
    }
    return std::nullopt;
}

SCENARIO("NMEA-0183 Message Framing", "[framer]") {
    GIVEN("A NMEA-0183 framer and a buffer") {
        std::array<char, 256> buffer;
        std::span<char> buffer_span(buffer);
        auto framer = nmea0183::create_framer(&buffer_span);

        WHEN("a complete and valid NMEA message is pushed") {
            const std::string_view message = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

            auto result = push_string(framer, message);

            THEN("the framer yields the correct parsed view") {
                REQUIRE(result.has_value());
                REQUIRE(result->has_value());
                const auto& view = result->value();

                CHECK(std::string(view.talker_id) == "GP"s);
                CHECK(std::string(view.message_type) == "GGA"s);
                REQUIRE(view.field_count == 14);
                CHECK(std::string(view.fields[0]) == "123519"s);
                CHECK(std::string(view.fields[1]) == "4807.038"s);
                CHECK(std::string(view.fields[2]) == "N"s);
                CHECK(std::string(view.fields[3]) == "01131.000"s);
                CHECK(std::string(view.fields[4]) == "E"s);
                CHECK(std::string(view.fields[5]) == "1"s);
                CHECK(std::string(view.fields[6]) == "08"s);
                CHECK(std::string(view.fields[7]) == "0.9"s);
                CHECK(std::string(view.fields[8]) == "545.4"s);
                CHECK(std::string(view.fields[9]) == "M"s);
                CHECK(std::string(view.fields[10]) == "46.9"s);
                CHECK(std::string(view.fields[11]) == "M"s);
                CHECK(std::string(view.fields[12]) == ""s);
                CHECK(std::string(view.fields[13]) == ""s);
            }
        }

        WHEN("a message with a mismatched checksum is pushed") {
            const std::string_view message =
                "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*48\r\n";  // Correct is *47

            auto result = push_string(framer, message);

            THEN("the framer yields a checksum mismatch error") {
                REQUIRE(result.has_value());
                REQUIRE_FALSE(result->has_value());
                CHECK(result->error().first == static_cast<int>(nmea0183::ErrorCode::CHECKSUM_MISMATCH));
                CHECK(std::string(result->error().second) == std::string(nmea0183::MSG_MISMATCH));
            }
        }

        WHEN("a message with an invalid checksum character is pushed") {
            const std::string_view message =
                "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*4G\r\n";  // 'G' is not a valid hex char

            auto result = push_string(framer, message);

            THEN("the framer yields an invalid checksum character error") {
                REQUIRE(result.has_value());
                REQUIRE_FALSE(result->has_value());
                CHECK(result->error().first == static_cast<int>(nmea0183::ErrorCode::INVALID_CHECKSUM_CHAR));
                CHECK(std::string(result->error().second) == std::string(nmea0183::MSG_INV_CHAR));
            }
        }

        WHEN("a message is pushed that would overrun the buffer") {
            std::array<char, 10> small_buffer;
            std::span<char> small_buffer_span(small_buffer);
            auto small_framer = nmea0183::create_framer(&small_buffer_span);
            const std::string_view message = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

            auto result = push_string(small_framer, message);

            THEN("the framer yields a buffer overrun error") {
                REQUIRE(result.has_value());
                REQUIRE_FALSE(result->has_value());
                CHECK(result->error().first == static_cast<int>(nmea0183::ErrorCode::BUFFER_OVERRUN));
                CHECK(std::string(result->error().second) == std::string(nmea0183::MSG_OVERRUN));
            }
        }

        WHEN("a message with a protocol violation (unexpected newline) is pushed") {
            const std::string_view message = "$GPGGA,123\n519*45\r\n";

            auto result = push_string(framer, message);

            THEN("the framer yields a protocol violation error") {
                REQUIRE(result.has_value());
                REQUIRE_FALSE(result->has_value());
                CHECK(result->error().first == static_cast<int>(nmea0183::ErrorCode::PROTOCOL_VIOLATION));
                CHECK(std::string(result->error().second) == std::string(nmea0183::MSG_PROTOCOL));
            }
        }

        WHEN("a message with a protocol violation (missing CRLF) is pushed") {
            const std::string_view message = "$GPGGA,123519*45\n\r";

            auto result = push_string(framer, message);

            THEN("the framer yields a protocol violation error") {
                REQUIRE(result.has_value());
                REQUIRE_FALSE(result->has_value());
                CHECK(result->error().first == static_cast<int>(nmea0183::ErrorCode::PROTOCOL_VIOLATION));
                CHECK(std::string(result->error().second) == std::string(nmea0183::MSG_BAD_CRLF));
            }
        }
    }
}
