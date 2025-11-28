#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/local_position_ned.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("LocalPositionNed Serialization", "[mavlink][local_position_ned]") {
    GIVEN("A populated LocalPositionNed message") {
        LocalPositionNed msg;
        msg.time_boot_ms.value = 1000;
        msg.x.value = 10.0f;
        msg.y.value = 20.0f;
        msg.z.value = 30.0f;
        msg.vx.value = 1.0f;
        msg.vy.value = 2.0f;
        msg.vz.value = 3.0f;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 28 + CRC 2 = 40
                REQUIRE(len == 40);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 28);  // LEN
                    CHECK(buffer[7] == 32);  // MSGID
                }
            }
        }
    }
}

SCENARIO("LocalPositionNed Deserialization", "[mavlink][local_position_ned]") {
    GIVEN("A buffer containing a LocalPositionNed payload") {
        LocalPositionNed msg_in;
        msg_in.time_boot_ms.value = 5000;
        msg_in.x.value = 100.0f;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 32;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<LocalPositionNed>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_boot_ms.value == 5000);
                CHECK(msg.x.value == 100.0f);
            }
        }
    }
}
