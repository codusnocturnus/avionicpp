#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/enumerations/mav_cmd.hpp"
#include "mavlink/enumerations/mav_frame.hpp"
#include "mavlink/payloads/command_int.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("CommandInt Serialization", "[mavlink][command_int]") {
    GIVEN("A populated CommandInt message") {
        CommandInt msg;
        msg.param1.value = 1.0f;
        msg.param2.value = 2.0f;
        msg.param3.value = 3.0f;
        msg.param4.value = 4.0f;
        msg.x.value = 500;
        msg.y.value = 600;
        msg.z.value = 7.0f;
        msg.command.value = MavCmd::NAV_TAKEOFF;
        msg.target_system.value = 1;
        msg.target_component.value = 1;
        msg.frame.value = MavFrame::GLOBAL_RELATIVE_ALT;
        msg.current.value = 0;
        msg.autocontinue.value = 1;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 35 + CRC 2 = 47
                REQUIRE(len == 47);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 35);  // LEN
                    CHECK(buffer[7] == 75);  // MSGID
                }
            }
        }
    }
}

SCENARIO("CommandInt Deserialization", "[mavlink][command_int]") {
    GIVEN("A buffer containing a CommandInt payload") {
        CommandInt msg_in;
        msg_in.command.value = MavCmd::NAV_TAKEOFF;
        msg_in.x.value = 1234567;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 75;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<CommandInt>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.command.value == MavCmd::NAV_TAKEOFF);
                CHECK(msg.x.value == 1234567);
            }
        }
    }
}
