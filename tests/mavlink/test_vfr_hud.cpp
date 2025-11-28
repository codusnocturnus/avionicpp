#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/vfr_hud.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("VfrHud Serialization", "[mavlink][vfr_hud]") {
    GIVEN("A populated VfrHud message") {
        VfrHud vfr;
        vfr.airspeed.value = 20.5f;     // 20.5 m/s
        vfr.groundspeed.value = 15.2f;  // 15.2 m/s
        vfr.alt.value = 100.0f;         // 100 m
        vfr.climb.value = 1.5f;         // 1.5 m/s
        vfr.heading.value = 180;        // 180 degrees (South)
        vfr.throttle.value = 75;        // 75%

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(vfr, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 20 - 1 (truncated zero) + CRC 2 = 31
                REQUIRE(len == 31);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 19);  // LEN
                    CHECK(buffer[7] == 74);  // MSGID 74
                }

                AND_THEN("the payload is correct") {
                    // Check float values roughly or by bytes if we want exactness.
                    // For now, let's assume if deserialization works, serialization was correct.
                    // But we can check integers easily.

                    // heading 180 (0xB4 00) at offset 16
                    CHECK(buffer[10 + 16] == 0xB4);
                    CHECK(buffer[10 + 17] == 0x00);

                    // throttle 75 (0x4B 00) at offset 18
                    CHECK(buffer[10 + 18] == 0x4B);
                    // Last byte truncated
                }
            }
        }
    }
}

SCENARIO("VfrHud Deserialization", "[mavlink][vfr_hud]") {
    GIVEN("A buffer containing a VfrHud payload") {
        // We can use the serializer to generate a valid payload buffer first to avoid manual byte calculation errors
        VfrHud vfr_in;
        vfr_in.airspeed.value = 20.5f;
        vfr_in.groundspeed.value = 15.2f;
        vfr_in.alt.value = 100.0f;
        vfr_in.climb.value = 1.5f;
        vfr_in.heading.value = 180;
        vfr_in.throttle.value = 75;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(vfr_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        // Extract payload bytes (skip header 10)
        std::size_t payload_len = *res - 12;  // -10 header -2 CRC
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 74;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<VfrHud>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& vfr = *res_d;
                CHECK_THAT(vfr.airspeed.value, Catch::Matchers::WithinRel(20.5f));
                CHECK_THAT(vfr.groundspeed.value, Catch::Matchers::WithinRel(15.2f));
                CHECK_THAT(vfr.alt.value, Catch::Matchers::WithinRel(100.0f));
                CHECK_THAT(vfr.climb.value, Catch::Matchers::WithinRel(1.5f));
                CHECK(vfr.heading.value == 180);
                CHECK(vfr.throttle.value == 75);
            }
        }
    }
}
