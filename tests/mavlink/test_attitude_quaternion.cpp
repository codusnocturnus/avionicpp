#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/attitude_quaternion.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("AttitudeQuaternion Serialization", "[mavlink][attitude_quaternion]") {
    GIVEN("A populated AttitudeQuaternion message") {
        AttitudeQuaternion msg;
        msg.time_boot_ms.value = 1000;
        msg.q1.value = 1.0f;
        msg.q2.value = 0.0f;
        msg.q3.value = 0.0f;
        msg.q4.value = 0.0f;
        msg.rollspeed.value = 0.1f;
        msg.pitchspeed.value = 0.2f;
        msg.yawspeed.value = 0.3f;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 32 + CRC 2 = 44
                REQUIRE(len == 44);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 32);  // LEN
                    CHECK(buffer[7] == 31);  // MSGID
                }
            }
        }
    }
}

SCENARIO("AttitudeQuaternion Deserialization", "[mavlink][attitude_quaternion]") {
    GIVEN("A buffer containing a AttitudeQuaternion payload") {
        AttitudeQuaternion msg_in;
        msg_in.time_boot_ms.value = 5000;
        msg_in.q1.value = 0.5f;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 31;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<AttitudeQuaternion>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_boot_ms.value == 5000);
                CHECK(msg.q1.value == 0.5f);
            }
        }
    }
}
