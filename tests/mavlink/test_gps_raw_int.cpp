#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/gps_raw_int.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("GpsRawInt Serialization", "[mavlink][gps_raw_int]") {
    GIVEN("A populated GpsRawInt message") {
        GpsRawInt gps;
        gps.time_usec.value = 1000000;  // 1s
        gps.fix_type.value = MavGpsFixType::FIX_3D;
        gps.lat.value = 473600000;  // 47.36 deg
        gps.lon.value = 85500000;   // 8.55 deg
        gps.alt.value = 500000;     // 500m
        gps.eph.value = 100;        // 1.00
        gps.epv.value = 200;        // 2.00
        gps.vel.value = 1500;       // 15 m/s
        gps.cog.value = 18000;      // 180 deg
        gps.satellites_visible.value = 10;
        gps.alt_ellipsoid.value = 550000;  // 550m
        gps.h_acc.value = 500;             // 0.5m
        gps.v_acc.value = 800;             // 0.8m
        gps.vel_acc.value = 20;            // 0.2m/s
        gps.hdg_acc.value = 1000;          // 1.0 deg
        gps.yaw.value = 9000;              // 90 deg

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(gps, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 52 + CRC 2 = 64
                // Payload size breakdown:
                // 1x uint64 (8) = 8
                // 4x int32 (4) = 16
                // 4x uint32 (4) = 16
                // 5x uint16 (2) = 10
                // 2x uint8 (1) = 2
                // Total: 8+16+16+10+2 = 52 bytes.
                REQUIRE(len == 64);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 52);  // LEN
                    CHECK(buffer[7] == 24);  // MSGID 24
                }

                AND_THEN("the payload is correct") {
                    // time_usec (8 bytes)
                    // lat (4 bytes)
                    // lon (4 bytes)
                    // alt (4 bytes)
                    // alt_ellipsoid (4 bytes)
                    // h_acc (4 bytes)
                    // v_acc (4 bytes)
                    // vel_acc (4 bytes)
                    // hdg_acc (4 bytes)
                    // eph (2 bytes)
                    // epv (2 bytes)
                    // vel (2 bytes)
                    // cog (2 bytes)
                    // yaw (2 bytes)
                    // fix_type (1 byte)
                    // satellites_visible (1 byte)

                    // fix_type is at offset 8+4*8 + 5*2 = 8+32+10 = 50
                    CHECK(buffer[10 + 50] == MavGpsFixType::FIX_3D);

                    // satellites_visible at 51
                    CHECK(buffer[10 + 51] == 10);
                }
            }
        }
    }
}

SCENARIO("GpsRawInt Deserialization", "[mavlink][gps_raw_int]") {
    GIVEN("A buffer containing a GpsRawInt payload") {
        GpsRawInt gps_in;
        gps_in.time_usec.value = 1000000;
        gps_in.fix_type.value = MavGpsFixType::FIX_3D;
        gps_in.lat.value = 473600000;
        gps_in.lon.value = 85500000;
        gps_in.alt.value = 500000;
        gps_in.eph.value = 100;
        gps_in.epv.value = 200;
        gps_in.vel.value = 1500;
        gps_in.cog.value = 18000;
        gps_in.satellites_visible.value = 10;
        gps_in.alt_ellipsoid.value = 550000;
        gps_in.h_acc.value = 500;
        gps_in.v_acc.value = 800;
        gps_in.vel_acc.value = 20;
        gps_in.hdg_acc.value = 1000;
        gps_in.yaw.value = 9000;

        std::array<std::uint8_t, 280> temp_buffer;
        auto res = serialize(gps_in, 1, 1, 0, temp_buffer);
        REQUIRE(res.has_value());

        std::size_t payload_len = *res - 12;
        std::vector<std::uint8_t> payload(temp_buffer.begin() + 10, temp_buffer.begin() + 10 + payload_len);

        MessageView view;
        view.msgid = 24;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res_d = deserialize<GpsRawInt>(view);

            THEN("the message matches the payload") {
                REQUIRE(res_d.has_value());
                const auto& gps = *res_d;
                CHECK(gps.time_usec.value == 1000000);
                CHECK(gps.fix_type.value == MavGpsFixType::FIX_3D);
                CHECK(gps.lat.value == 473600000);
                CHECK(gps.lon.value == 85500000);
                CHECK(gps.alt.value == 500000);
                CHECK(gps.eph.value == 100);
                CHECK(gps.epv.value == 200);
                CHECK(gps.vel.value == 1500);
                CHECK(gps.cog.value == 18000);
                CHECK(gps.satellites_visible.value == 10);
                CHECK(gps.alt_ellipsoid.value == 550000);
                CHECK(gps.h_acc.value == 500);
                CHECK(gps.v_acc.value == 800);
                CHECK(gps.vel_acc.value == 20);
                CHECK(gps.hdg_acc.value == 1000);
                CHECK(gps.yaw.value == 9000);
            }
        }
    }
}
