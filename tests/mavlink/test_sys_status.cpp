#include <array>
#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/sys_status.hpp"
#include "mavlink/serializer.hpp"

using namespace mavlink;
using namespace mavlink::payloads;
using namespace mavlink::enumerations;

SCENARIO("SysStatus Serialization", "[mavlink][sys_status]") {
    GIVEN("A populated SysStatus message") {
        SysStatus ss;
        ss.onboard_control_sensors_present.value = MavSysStatusSensor::GYRO_3D | MavSysStatusSensor::ACCEL_3D;
        ss.onboard_control_sensors_enabled.value = MavSysStatusSensor::GYRO_3D;
        ss.onboard_control_sensors_health.value = MavSysStatusSensor::GYRO_3D;
        ss.load.value = 500;               // 50%
        ss.voltage_battery.value = 11100;  // 11.1V
        ss.current_battery.value = 1000;   // 1A
        ss.drop_rate_comm.value = 10;
        ss.errors_comm.value = 5;
        ss.errors_count1.value = 1;
        ss.errors_count2.value = 2;
        ss.errors_count3.value = 3;
        ss.errors_count4.value = 4;
        ss.battery_remaining.value = 85;  // 85%

        std::array<std::uint8_t, 280> buffer;

        WHEN("serialized") {
            auto res = serialize(ss, 1, 1, 0, buffer);

            THEN("the serialization result is successful") {
                REQUIRE(res.has_value());

                std::size_t len = *res;
                // Header 10 + Payload 31 + CRC 2 = 43
                REQUIRE(len == 43);

                AND_THEN("the header is correct") {
                    CHECK(buffer[0] == 0xFD);
                    CHECK(buffer[1] == 31);  // LEN
                    CHECK(buffer[7] == 1);   // MSGID 1
                }

                AND_THEN("the payload is correct") {
                    // onboard_control_sensors_present (3D_GYRO|3D_ACCEL = 3) -> 03 00 00 00
                    CHECK(buffer[10] == 0x03);
                    CHECK(buffer[11] == 0x00);
                    CHECK(buffer[12] == 0x00);
                    CHECK(buffer[13] == 0x00);

                    // onboard_control_sensors_enabled (3D_GYRO = 1) -> 01 00 00 00
                    CHECK(buffer[14] == 0x01);
                    CHECK(buffer[15] == 0x00);
                    CHECK(buffer[16] == 0x00);
                    CHECK(buffer[17] == 0x00);

                    // onboard_control_sensors_health (3D_GYRO = 1) -> 01 00 00 00
                    CHECK(buffer[18] == 0x01);
                    CHECK(buffer[19] == 0x00);
                    CHECK(buffer[20] == 0x00);
                    CHECK(buffer[21] == 0x00);

                    // load 500 -> F4 01
                    CHECK(buffer[22] == 0xF4);
                    CHECK(buffer[23] == 0x01);

                    // voltage_battery 11100 -> 5C 2B
                    CHECK(buffer[24] == 0x5C);
                    CHECK(buffer[25] == 0x2B);

                    // current_battery 1000 -> E8 03
                    CHECK(buffer[26] == 0xE8);
                    CHECK(buffer[27] == 0x03);

                    // drop_rate_comm 10 -> 0A 00
                    CHECK(buffer[28] == 0x0A);
                    CHECK(buffer[29] == 0x00);

                    // errors_comm 5 -> 05 00
                    CHECK(buffer[30] == 0x05);
                    CHECK(buffer[31] == 0x00);

                    // errors_count1 1 -> 01 00
                    CHECK(buffer[32] == 0x01);
                    CHECK(buffer[33] == 0x00);

                    // errors_count2 2 -> 02 00
                    CHECK(buffer[34] == 0x02);
                    CHECK(buffer[35] == 0x00);

                    // errors_count3 3 -> 03 00
                    CHECK(buffer[36] == 0x03);
                    CHECK(buffer[37] == 0x00);

                    // errors_count4 4 -> 04 00
                    CHECK(buffer[38] == 0x04);
                    CHECK(buffer[39] == 0x00);

                    // battery_remaining 85 -> 55
                    CHECK(buffer[40] == 0x55);
                }
            }
        }
    }
}

SCENARIO("SysStatus Deserialization", "[mavlink][sys_status]") {
    GIVEN("A buffer containing a SysStatus payload") {
        std::array<std::uint8_t, 31> payload = {
            0x03, 0x00, 0x00, 0x00,  // present
            0x01, 0x00, 0x00, 0x00,  // enabled
            0x01, 0x00, 0x00, 0x00,  // health
            0xF4, 0x01,              // load
            0x5C, 0x2B,              // voltage
            0xE8, 0x03,              // current
            0x0A, 0x00,              // drop
            0x05, 0x00,              // errors comm
            0x01, 0x00,              // errors1
            0x02, 0x00,              // errors2
            0x03, 0x00,              // errors3
            0x04, 0x00,              // errors4
            0x55                     // battery
        };

        MessageView view;
        view.msgid = 1;
        view.payload = std::span<const std::uint8_t>(payload);

        WHEN("deserialized") {
            auto res = deserialize<SysStatus>(view);

            THEN("the message matches the payload") {
                REQUIRE(res.has_value());
                const auto& ss = *res;
                CHECK(ss.onboard_control_sensors_present.value ==
                      (MavSysStatusSensor::GYRO_3D | MavSysStatusSensor::ACCEL_3D));
                CHECK(ss.onboard_control_sensors_enabled.value == MavSysStatusSensor::GYRO_3D);
                CHECK(ss.onboard_control_sensors_health.value == MavSysStatusSensor::GYRO_3D);
                CHECK(ss.load.value == 500);
                CHECK(ss.voltage_battery.value == 11100);
                CHECK(ss.current_battery.value == 1000);
                CHECK(ss.drop_rate_comm.value == 10);
                CHECK(ss.errors_comm.value == 5);
                CHECK(ss.errors_count1.value == 1);
                CHECK(ss.errors_count2.value == 2);
                CHECK(ss.errors_count3.value == 3);
                CHECK(ss.errors_count4.value == 4);
                CHECK(ss.battery_remaining.value == 85);
            }
        }
    }
}
