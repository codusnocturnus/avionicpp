#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/heartbeat.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("Heartbeat Serialization", "[mavlink][heartbeat]") {
    GIVEN("A populated Heartbeat message") {
        Heartbeat hb;
        hb.custom_mode.value = 0xDEADBEEF;
        hb.type.value = MavType::FIXED_WING;
        hb.autopilot.value = MavAutopilot::SLUGS;
        hb.base_mode.value = MavModeFlag::CUSTOM_MODE_ENABLED | MavModeFlag::TEST_ENABLED;
        hb.system_status.value = MavState::ACTIVE;
        hb.mavlink_version.value = 5;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized with specific system and component IDs") {
            // SysID 1, CompID 1, Seq 0
            auto res = serialize(hb, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 9 + CRC 2 = 21
                REQUIRE(len == 21);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);  // STX
                    CHECK(buffer[1] == 9);     // LEN
                    CHECK(buffer[2] == 0);     // INC
                    CHECK(buffer[3] == 0);     // CMP
                    CHECK(buffer[4] == 0);     // SEQ
                    CHECK(buffer[5] == 1);     // SYS
                    CHECK(buffer[6] == 1);     // COMP
                    // MSGID 0
                    CHECK(buffer[7] == 0);
                    CHECK(buffer[8] == 0);
                    CHECK(buffer[9] == 0);
                }

                AND_THEN("the payload is correct") {
                    // custom_mode (LE)
                    CHECK(buffer[10] == 0xEF);
                    CHECK(buffer[11] == 0xBE);
                    CHECK(buffer[12] == 0xAD);
                    CHECK(buffer[13] == 0xDE);
                    // type
                    CHECK(buffer[14] == MavType::FIXED_WING);
                    // autopilot
                    CHECK(buffer[15] == MavAutopilot::SLUGS);
                    // base_mode
                    CHECK(buffer[16] == (MavModeFlag::CUSTOM_MODE_ENABLED | MavModeFlag::TEST_ENABLED));
                    // system_status
                    CHECK(buffer[17] == MavState::ACTIVE);
                    // mavlink_version
                    CHECK(buffer[18] == 5);
                }
            }
        }
    }
}

SCENARIO("Heartbeat Deserialization", "[mavlink][heartbeat]") {
    GIVEN("A buffer containing a Heartbeat payload") {
        std::array<std::uint8_t, 9> payload = {
            0xEF,
            0xBE,
            0xAD,
            0xDE,                                                          // custom_mode
            MavType::FIXED_WING,                                           // type
            MavAutopilot::SLUGS,                                           // autopilot
            MavModeFlag::CUSTOM_MODE_ENABLED | MavModeFlag::TEST_ENABLED,  // base_mode
            MavState::ACTIVE,                                              // system_status
            5                                                              // mavlink_version
        };

        MessageView view;
        view.msgid = 0;  // Heartbeat
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<Heartbeat>(view);

            THEN("the message matches the payload") {
                REQUIRE(res.has_value());
                const auto& hb = *res;
                CHECK(hb.custom_mode.value == 0xDEADBEEF);
                CHECK(hb.type.value == MavType::FIXED_WING);
                CHECK(hb.autopilot.value == MavAutopilot::SLUGS);
                CHECK(hb.base_mode.value == (MavModeFlag::CUSTOM_MODE_ENABLED | MavModeFlag::TEST_ENABLED));
                CHECK(hb.system_status.value == MavState::ACTIVE);
                CHECK(hb.mavlink_version.value == 5);
            }
        }
    }

    GIVEN("A truncated buffer (zero truncation)") {
        std::array<std::uint8_t, 4> payload = {
            0xEF, 0xBE, 0xAD, 0xDE  // custom_mode only
            // rest zeros implied
        };

        MessageView view;
        view.msgid = 0;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<Heartbeat>(view);

            THEN("missing fields are zero-initialized") {
                REQUIRE(res.has_value());
                const auto& hb = *res;
                CHECK(hb.custom_mode.value == 0xDEADBEEF);
                CHECK(hb.type.value == MavType::GENERIC);
                CHECK(hb.autopilot.value == MavAutopilot::GENERIC);
            }
        }
    }
}
