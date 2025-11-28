#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/autopilot_version.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("AutopilotVersion Serialization", "[mavlink][autopilot_version]") {
    GIVEN("A populated AutopilotVersion message") {
        AutopilotVersion ap;
        ap.capabilities.value = MavProtocolCapability::MISSION_FLOAT | MavProtocolCapability::PARAM_FLOAT;
        ap.uid.value = 0x0102030405060708;
        ap.flight_sw_version.value = 100;
        ap.middleware_sw_version.value = 200;
        ap.os_sw_version.value = 300;
        ap.board_version.value = 400;
        ap.vendor_id.value = 10;
        ap.product_id.value = 20;

        ap.flight_custom_version.value = {1, 2, 3, 4, 5, 6, 7, 8};
        ap.middleware_custom_version.value = {8, 7, 6, 5, 4, 3, 2, 1};
        ap.os_custom_version.value = {0, 0, 0, 0, 0, 0, 0, 0};

        std::array<std::uint8_t, 18> uid2 = {0};
        uid2[0] = 0xFF;
        uid2[17] = 0xEE;
        ap.uid2.value = uid2;

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(ap, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 78 + CRC 2 = 90
                // Payload:
                // capabilities (8)
                // uid (8)
                // flight_sw_version (4)
                // middleware_sw_version (4)
                // os_sw_version (4)
                // board_version (4)
                // vendor_id (2)
                // product_id (2)
                // flight_custom_version (8)
                // middleware_custom_version (8)
                // os_custom_version (8)
                // uid2 (18)
                // Total: 8+8+4+4+4+4+2+2+8+8+8+18 = 78
                REQUIRE(len == 90);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 78);   // LEN
                    CHECK(buffer[7] == 148);  // MSGID 148
                }

                AND_THEN("the payload is correct") {
                    // Check capabilities (offset 0)
                    // MISSION_FLOAT (1) | PARAM_FLOAT (2) = 3
                    CHECK(buffer[10 + 0] == 3);

                    // Check uid2 (offset 78 - 18 = 60)
                    CHECK(buffer[10 + 60] == 0xFF);
                    CHECK(buffer[10 + 60 + 17] == 0xEE);
                }
            }
        }
    }
}

SCENARIO("AutopilotVersion Deserialization", "[mavlink][autopilot_version]") {
    GIVEN("A buffer containing a AutopilotVersion payload") {
        AutopilotVersion ap_in;
        ap_in.capabilities.value = MavProtocolCapability::MISSION_FLOAT;
        ap_in.uid.value = 0x0102030405060708;
        ap_in.flight_sw_version.value = 100;
        ap_in.middleware_sw_version.value = 200;
        ap_in.os_sw_version.value = 300;
        ap_in.board_version.value = 400;
        ap_in.vendor_id.value = 10;
        ap_in.product_id.value = 20;
        ap_in.flight_custom_version.value = {1, 2, 3, 4, 5, 6, 7, 8};
        ap_in.middleware_custom_version.value = {8, 7, 6, 5, 4, 3, 2, 1};
        ap_in.os_custom_version.value = {0};

        std::array<std::uint8_t, 18> uid2 = {0};
        uid2[0] = 0xAA;
        ap_in.uid2.value = uid2;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(ap_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 148;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<AutopilotVersion>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& ap = *res_d;
                CHECK(ap.capabilities.value == MavProtocolCapability::MISSION_FLOAT);
                CHECK(ap.uid.value == 0x0102030405060708);
                CHECK(ap.flight_sw_version.value == 100);
                CHECK(ap.uid2.value[0] == 0xAA);
            }
        }
    }
}
