#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/scaled_imu.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ScaledImu Serialization", "[mavlink][scaled_imu]") {
    GIVEN("A populated ScaledImu message") {
        ScaledImu msg;
        msg.time_boot_ms.value = 1000;
        msg.xacc.value = 10;
        msg.yacc.value = 20;
        msg.zacc.value = 30;
        msg.xgyro.value = 40;
        msg.ygyro.value = 50;
        msg.zgyro.value = 60;
        msg.xmag.value = 70;
        msg.ymag.value = 80;
        msg.zmag.value = 90;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 22 + CRC 2 = 34
                // Truncated: zmag (int16) is 90 (0x005A). Last byte is 00.
                // So 1 byte truncated. Length 33.
                REQUIRE(len == 33);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 21);  // LEN (Truncated)
                    CHECK(buffer[7] == 26);  // MSGID
                }
            }
        }
    }
}

SCENARIO("ScaledImu Deserialization", "[mavlink][scaled_imu]") {
    GIVEN("A buffer containing a ScaledImu payload") {
        ScaledImu msg_in;
        msg_in.time_boot_ms.value = 5000;
        msg_in.xacc.value = 123;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 26;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ScaledImu>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_boot_ms.value == 5000);
                CHECK(msg.xacc.value == 123);
            }
        }
    }
}
