#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/rc_channels_scaled.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("RcChannelsScaled Serialization", "[mavlink][rc_channels_scaled]") {
    GIVEN("A populated RcChannelsScaled message") {
        RcChannelsScaled msg;
        msg.time_boot_ms.value = 1000;
        msg.port.value = 1;
        msg.chan1_scaled.value = 1000;
        msg.chan2_scaled.value = 2000;
        msg.chan3_scaled.value = 3000;
        msg.chan4_scaled.value = 4000;
        msg.chan5_scaled.value = 5000;
        msg.chan6_scaled.value = 6000;
        msg.chan7_scaled.value = 7000;
        msg.chan8_scaled.value = 8000;
        msg.rssi.value = 100;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 22 + CRC 2 = 34
                REQUIRE(len == 34);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 22);  // LEN
                    CHECK(buffer[7] == 34);  // MSGID
                }
            }
        }
    }
}

SCENARIO("RcChannelsScaled Deserialization", "[mavlink][rc_channels_scaled]") {
    GIVEN("A buffer containing a RcChannelsScaled payload") {
        RcChannelsScaled msg_in;
        msg_in.time_boot_ms.value = 5000;
        msg_in.chan1_scaled.value = 1500;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 34;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<RcChannelsScaled>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_boot_ms.value == 5000);
                CHECK(msg.chan1_scaled.value == 1500);
            }
        }
    }
}
