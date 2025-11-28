#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/scaled_pressure.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("ScaledPressure Serialization", "[mavlink][scaled_pressure]") {
    GIVEN("A populated ScaledPressure message") {
        ScaledPressure msg;
        msg.time_boot_ms.value = 1000;
        msg.press_abs.value = 1013.25f;
        msg.press_diff.value = 0.5f;
        msg.temperature.value = 2500;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 14 + CRC 2 = 26
                REQUIRE(len == 26);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 14);  // LEN
                    CHECK(buffer[7] == 29);  // MSGID
                }
            }
        }
    }
}

SCENARIO("ScaledPressure Deserialization", "[mavlink][scaled_pressure]") {
    GIVEN("A buffer containing a ScaledPressure payload") {
        ScaledPressure msg_in;
        msg_in.time_boot_ms.value = 5000;
        msg_in.press_abs.value = 1000.0f;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 29;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<ScaledPressure>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.time_boot_ms.value == 5000);
                CHECK(msg.press_abs.value == 1000.0f);
            }
        }
    }
}
