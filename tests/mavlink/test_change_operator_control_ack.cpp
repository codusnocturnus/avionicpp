#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/change_operator_control_ack.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ChangeOperatorControlAck Serialization", "[mavlink][change_operator_control_ack]") {
    GIVEN("A populated ChangeOperatorControlAck message") {
        ChangeOperatorControlAck ack;
        ack.gcs_system_id.value = 255;
        ack.control_request.value = 0;
        ack.ack.value = 0;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(ack, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 3 + CRC 2 = 15
                // Truncated: control_request=0, ack=0.
                // Payload size: 1 (gcs_system_id).
                // Total: 10 + 1 + 2 = 13.
                REQUIRE(len == 13);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 1);  // LEN
                    CHECK(buffer[7] == 6);  // MSGID 6
                }

                AND_THEN("the payload is correct") {
                    // gcs_system_id (offset 0)
                    CHECK(buffer[10] == 255);
                }
            }
        }
    }
}

SCENARIO("ChangeOperatorControlAck Deserialization", "[mavlink][change_operator_control_ack]") {
    GIVEN("A buffer containing a ChangeOperatorControlAck payload") {
        std::array<std::uint8_t, 3> payload = {
            255,  // gcs_system_id
            0,    // control_request
            0     // ack
        };

        MessageView view;
        view.msgid = 6;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ChangeOperatorControlAck>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& ack = *res_d;
                CHECK(ack.gcs_system_id.value == 255);
                CHECK(ack.control_request.value == 0);
                CHECK(ack.ack.value == 0);
            }
        }
    }
}
