#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/gps_status.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("GpsStatus Serialization", "[mavlink][gps_status]") {
    GIVEN("A populated GpsStatus message") {
        GpsStatus msg;
        msg.satellites_visible.value = 10;
        msg.satellite_prn.value.fill(1);
        msg.satellite_used.value.fill(2);
        msg.satellite_elevation.value.fill(3);
        msg.satellite_azimuth.value.fill(4);
        msg.satellite_snr.value.fill(5);

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(msg, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 101 + CRC 2 = 113
                REQUIRE(len == 113);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 101);  // LEN
                    CHECK(buffer[7] == 25);   // MSGID
                }
            }
        }
    }
}

SCENARIO("GpsStatus Deserialization", "[mavlink][gps_status]") {
    GIVEN("A buffer containing a GpsStatus payload") {
        GpsStatus msg_in;
        msg_in.satellites_visible.value = 5;
        msg_in.satellite_prn.value.fill(10);

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(msg_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        // Payload starts at index 10. Length is at index 1.
        std::size_t payload_len = temp_buffer[1];
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 25;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<GpsStatus>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& msg = *res_d;
                CHECK(msg.satellites_visible.value == 5);
                CHECK(msg.satellite_prn.value[0] == 10);
            }
        }
    }
}
