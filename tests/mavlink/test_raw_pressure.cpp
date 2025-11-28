#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/raw_pressure.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("RawPressure Serialization", "[mavlink][raw_pressure]") {
    GIVEN("A populated RawPressure message") {
        RawPressure msg;
        msg.time_usec.value = 2000000;
        msg.press_abs.value = 100;
        msg.press_diff1.value = 200;
        msg.press_diff2.value = 300;
        msg.temperature.value = 400;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 16 + CRC 2 = 28
                REQUIRE(len == 28);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 16);  // LEN
                    CHECK(buffer[7] == 28);  // MSGID
                }
            }
        }
    }
}

SCENARIO("RawPressure Deserialization", "[mavlink][raw_pressure]") {
    GIVEN("A buffer containing a RawPressure payload") {
        RawPressure msg_in;
        msg_in.time_usec.value = 6000000;
        msg_in.press_abs.value = 500;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 28;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<RawPressure>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_usec.value == 6000000);
                CHECK(msg.press_abs.value == 500);
            }
        }
    }
}
