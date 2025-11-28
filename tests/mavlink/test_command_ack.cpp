#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/enumerations/mav_cmd.hpp"
#include "mavlink/enumerations/mav_result.hpp"
#include "mavlink/payloads/command_ack.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("CommandAck Serialization", "[mavlink][command_ack]") {
    GIVEN("A populated CommandAck message") {
        CommandAck msg;
        msg.command.value = MavCmd::NAV_WAYPOINT;
        msg.result.value = MavResult::ACCEPTED;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 3 + CRC 2 = 15
                // Truncated: result is 0, command high byte is 0. Payload becomes 1.
                // Total: 10 + 1 + 2 = 13.
                REQUIRE(len == 13);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 1);   // LEN (Truncated)
                    CHECK(buffer[7] == 77);  // MSGID
                }
            }
        }
    }
}

SCENARIO("CommandAck Deserialization", "[mavlink][command_ack]") {
    GIVEN("A buffer containing a CommandAck payload") {
        CommandAck msg_in;
        msg_in.command.value = MavCmd::NAV_TAKEOFF;
        msg_in.result.value = MavResult::FAILED;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 77;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<CommandAck>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.command.value == MavCmd::NAV_TAKEOFF);
                CHECK(msg.result.value == MavResult::FAILED);
            }
        }
    }
}
