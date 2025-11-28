#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/global_position_int.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;

SCENARIO("GlobalPositionInt Serialization", "[mavlink][global_position_int]") {
    GIVEN("A populated GlobalPositionInt message") {
        GlobalPositionInt gpos;
        gpos.time_boot_ms.value = 12345678;
        gpos.lat.value = 0x1C3A1600;      // 473600000
        gpos.lon.value = 85500000;        // 8.55 deg
        gpos.alt.value = 500000;          // 500m
        gpos.relative_alt.value = 50000;  // 50m
        gpos.vx.value = 1500;             // 15 m/s
        gpos.vy.value = 200;              // 2 m/s
        gpos.vz.value = -100;             // -1 m/s
        gpos.hdg.value = 18000;           // 180 deg

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(gpos, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 28 + CRC 2 = 40
                // Payload size:
                // time_boot_ms: 4
                // lat: 4
                // lon: 4
                // alt: 4
                // relative_alt: 4
                // vx: 2
                // vy: 2
                // vz: 2
                // hdg: 2
                // Total: 20 + 8 = 28
                REQUIRE(len == 40);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 28);  // LEN
                    CHECK(buffer[7] == 33);  // MSGID 33
                }

                AND_THEN("the payload is correct") {
                    // check lat (offset 4)
                    // 0x1C3A1600 -> 00 16 3A 1C
                    CHECK(static_cast<int>(buffer[10 + 4]) == 0x00);
                    CHECK(static_cast<int>(buffer[10 + 5]) == 0x16);
                    CHECK(static_cast<int>(buffer[10 + 6]) == 0x3A);
                    CHECK(static_cast<int>(buffer[10 + 7]) == 0x1C);

                    // check vx (offset 20)
                    // 1500 = 0x05DC -> DC 05
                    CHECK(static_cast<int>(buffer[10 + 20]) == 0xDC);
                    CHECK(static_cast<int>(buffer[10 + 21]) == 0x05);
                }
            }
        }
    }
}

SCENARIO("GlobalPositionInt Deserialization", "[mavlink][global_position_int]") {
    GIVEN("A buffer containing a GlobalPositionInt payload") {
        GlobalPositionInt gpos_in;
        gpos_in.time_boot_ms.value = 12345678;
        gpos_in.lat.value = 473600000;
        gpos_in.lon.value = 85500000;
        gpos_in.alt.value = 500000;
        gpos_in.relative_alt.value = 50000;
        gpos_in.vx.value = 1500;
        gpos_in.vy.value = 200;
        gpos_in.vz.value = -100;
        gpos_in.hdg.value = 18000;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(gpos_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 33;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<GlobalPositionInt>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& gpos = *res_d;
                CHECK(gpos.time_boot_ms.value == 12345678);
                CHECK(gpos.lat.value == 473600000);
                CHECK(gpos.lon.value == 85500000);
                CHECK(gpos.alt.value == 500000);
                CHECK(gpos.relative_alt.value == 50000);
                CHECK(gpos.vx.value == 1500);
                CHECK(gpos.vy.value == 200);
                CHECK(gpos.vz.value == -100);
                CHECK(gpos.hdg.value == 18000);
            }
        }
    }
}
