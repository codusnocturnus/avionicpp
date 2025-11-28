#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/attitude.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("Attitude Serialization", "[mavlink][attitude]") {
    GIVEN("A populated Attitude message") {
        Attitude att;
        att.time_boot_ms.value = 0x12345678;
        att.roll.value = 1.0f;         // 0x3F800000
        att.pitch.value = -1.0f;       // 0xBF800000
        att.yaw.value = 0.5f;          // 0x3F000000
        att.rollspeed.value = 0.1f;    // 0x3DCCCCCD
        att.pitchspeed.value = -0.1f;  // 0xBDCCCCCD
        att.yawspeed.value = 0.5f;     // 0x3F000000

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(att, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 28 (7*4) + CRC 2 = 40
                REQUIRE(len == 40);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 28);  // LEN
                    CHECK(buffer[7] == 30);  // MSGID 30
                }

                AND_THEN("the payload is correct") {
                    // time_boot_ms: 0x12345678 -> 78 56 34 12
                    CHECK(buffer[10] == 0x78);
                    CHECK(buffer[11] == 0x56);
                    CHECK(buffer[12] == 0x34);
                    CHECK(buffer[13] == 0x12);

                    // roll: 1.0f -> 00 00 80 3F
                    CHECK(buffer[14] == 0x00);
                    CHECK(buffer[15] == 0x00);
                    CHECK(buffer[16] == 0x80);
                    CHECK(buffer[17] == 0x3F);

                    // pitch: -1.0f -> 00 00 80 BF
                    CHECK(buffer[18] == 0x00);
                    CHECK(buffer[19] == 0x00);
                    CHECK(buffer[20] == 0x80);
                    CHECK(buffer[21] == 0xBF);

                    // yaw: 0.5f -> 00 00 00 3F
                    CHECK(buffer[22] == 0x00);
                    CHECK(buffer[23] == 0x00);
                    CHECK(buffer[24] == 0x00);
                    CHECK(buffer[25] == 0x3F);

                    // rollspeed: 0.1f -> CD CC CC 3D
                    CHECK(buffer[26] == 0xCD);
                    CHECK(buffer[27] == 0xCC);
                    CHECK(buffer[28] == 0xCC);
                    CHECK(buffer[29] == 0x3D);

                    // pitchspeed: -0.1f -> CD CC CC BD
                    CHECK(buffer[30] == 0xCD);
                    CHECK(buffer[31] == 0xCC);
                    CHECK(buffer[32] == 0xCC);
                    CHECK(buffer[33] == 0xBD);

                    // yawspeed: 0.5f -> 00 00 00 3F
                    CHECK(buffer[34] == 0x00);
                    CHECK(buffer[35] == 0x00);
                    CHECK(buffer[36] == 0x00);
                    CHECK(buffer[37] == 0x3F);
                }
            }
        }
    }
}

SCENARIO("Attitude Deserialization", "[mavlink][attitude]") {
    GIVEN("A buffer containing an Attitude payload") {
        std::array<std::uint8_t, 28> payload = {
            0x78, 0x56, 0x34, 0x12,  // time_boot_ms: 0x12345678
            0x00, 0x00, 0x80, 0x3F,  // roll: 1.0f
            0x00, 0x00, 0x80, 0xBF,  // pitch: -1.0f
            0x00, 0x00, 0x00, 0x3F,  // yaw: 0.5f
            0xCD, 0xCC, 0xCC, 0x3D,  // rollspeed: 0.1f
            0xCD, 0xCC, 0xCC, 0xBD,  // pitchspeed: -0.1f
            0x00, 0x00, 0x00, 0x3F   // yawspeed: 0.5f
        };

        MessageView view;
        view.msgid = 30;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<Attitude>(view);

            THEN("the message matches the payload") {
                REQUIRE(res.has_value());
                const auto& att = *res;
                CHECK(att.time_boot_ms.value == 0x12345678);
                CHECK_THAT(att.roll.value, Catch::Matchers::WithinRel(1.0f));
                CHECK_THAT(att.pitch.value, Catch::Matchers::WithinRel(-1.0f));
                CHECK_THAT(att.yaw.value, Catch::Matchers::WithinRel(0.5f));
                CHECK_THAT(att.rollspeed.value, Catch::Matchers::WithinRel(0.1f));
                CHECK_THAT(att.pitchspeed.value, Catch::Matchers::WithinRel(-0.1f));
                CHECK_THAT(att.yawspeed.value, Catch::Matchers::WithinRel(0.5f));
            }
        }
    }
}
