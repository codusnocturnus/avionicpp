#include <cstdint>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "mavlink/checksum.hpp"
#include "mavlink/payloads/heartbeat.hpp"
#include "mavlink/types.hpp"

using namespace mavlink;

// Define a test struct matching Heartbeat to verify logic on local types
struct TestHeartbeat {
    static constexpr std::string_view MessageName = "HEARTBEAT";
    // Wire order: custom_mode, type, autopilot, base_mode, system_status, mavlink_version
    std::uint32_t custom_mode;
    std::uint8_t type;
    std::uint8_t autopilot;
    std::uint8_t base_mode;
    std::uint8_t system_status;
    std::uint8_t mavlink_version;
};

// Define another known message: SYS_STATUS (Msg 1), CRC Extra 124
struct TestSysStatus {
    static constexpr std::string_view MessageName = "SYS_STATUS";
    // Wire order (sorted by type size):
    // u32 fields
    std::uint32_t onboard_control_sensors_present;
    std::uint32_t onboard_control_sensors_enabled;
    std::uint32_t onboard_control_sensors_health;
    // u16/s16 fields
    std::uint16_t load;
    std::uint16_t voltage_battery;
    std::int16_t current_battery;
    std::uint16_t drop_rate_comm;
    std::uint16_t errors_comm;
    std::uint16_t errors_count1;
    std::uint16_t errors_count2;
    std::uint16_t errors_count3;
    std::uint16_t errors_count4;
    // s8 fields
    std::int8_t battery_remaining;
};

SCENARIO("CRC Extra Calculation", "[mavlink][checksum]") {
    GIVEN("A Heartbeat message definition") {
        THEN("the calculated CRC Extra matches the known value (50)") {
            constexpr std::uint8_t crc_official = mavlink::payloads::Heartbeat::CrcExtra;
            CHECK(crc_official == 50);

            constexpr std::uint8_t crc_local = calculate_crc_extra<TestHeartbeat>();
            CHECK(crc_local == 50);
        }
    }

    GIVEN("A SysStatus message definition") {
        THEN("the calculated CRC Extra matches the known value (124)") {
            constexpr std::uint8_t crc = calculate_crc_extra<TestSysStatus>();
            CHECK(crc == 124);
        }
    }
}
