#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/enumerations/mav_cmd.hpp"
#include "mavlink/payloads/command_long.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("CommandLong Serialization", "[mavlink][command_long]") {
    GIVEN("A populated CommandLong message") {
        CommandLong msg;
        msg.param1.value = 1.0f;
        msg.param2.value = 2.0f;
        msg.param3.value = 3.0f;
        msg.param4.value = 4.0f;
        msg.param5.value = 5.0f;
        msg.param6.value = 6.0f;
        msg.param7.value = 7.0f;
        msg.command.value = MavCmd::NAV_WAYPOINT;
        msg.target_system.value = 1;
        msg.target_component.value = 1;
        msg.confirmation.value = 0;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 33 + CRC 2 = 45
                // Truncated: confirmation is 0. Payload becomes 32.
                // Total: 10 + 32 + 2 = 44.
                REQUIRE(len == 44);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 32);  // LEN (Truncated)
                    CHECK(buffer[7] == 76);  // MSGID
                }
            }
        }
    }
}

SCENARIO("CommandLong Deserialization", "[mavlink][command_long]") {
    GIVEN("A buffer containing a CommandLong payload") {
        CommandLong msg_in;
        msg_in.command.value = MavCmd::NAV_WAYPOINT;
        msg_in.param1.value = 1.0f;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 76;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<CommandLong>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.command.value == MavCmd::NAV_WAYPOINT);
                CHECK(msg.param1.value == 1.0f);
            }
        }
    }
}
