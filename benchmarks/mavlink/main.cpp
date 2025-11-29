#include <benchmark/benchmark.h>

#include <array>
#include <span>
#include <vector>

#include "mavlink/deserializer.hpp"
#include "mavlink/payloads/attitude.hpp"
#include "mavlink/payloads/sys_status.hpp"
#include "mavlink/serializer.hpp"

// C Library headers
// We need to ensure we don't conflict with any macros if any
#include <common/mavlink.h>

using namespace mavlink;
using namespace mavlink::payloads;

// --- Avionicpp Benchmarks ---

static void BM_Mavlink_Serialize_Attitude(benchmark::State& state) {
    Attitude att;
    att.time_boot_ms.value = 12345678;
    att.roll.value = 1.0f;
    att.pitch.value = -1.0f;
    att.yaw.value = 0.5f;
    att.rollspeed.value = 0.1f;
    att.pitchspeed.value = -0.1f;
    att.yawspeed.value = 0.5f;

    std::array<std::uint8_t, 280> buffer;

    for (auto _ : state) {
        auto res = serialize(att, 1, 1, 0, buffer);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(buffer);
    }
}
BENCHMARK(BM_Mavlink_Serialize_Attitude);

static void BM_Mavlink_Deserialize_Attitude(benchmark::State& state) {
    // Pre-serialize a message to deserialize
    Attitude att;
    att.time_boot_ms.value = 12345678;
    att.roll.value = 1.0f;
    att.pitch.value = -1.0f;
    att.yaw.value = 0.5f;
    att.rollspeed.value = 0.1f;
    att.pitchspeed.value = -0.1f;
    att.yawspeed.value = 0.5f;

    std::array<std::uint8_t, 280> buffer;
    auto res = serialize(att, 1, 1, 0, buffer);

    // Create MessageView for the payload
    // serialize writes: Header (10 bytes) + Payload (28 bytes) + CRC (2 bytes)
    // Payload starts at offset 10.
    MessageView view;
    view.msgid = Attitude::MessageId;
    view.payload = std::span<const std::uint8_t>(buffer.data() + 10, 28);

    for (auto _ : state) {
        auto res = deserialize<Attitude>(view);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(BM_Mavlink_Deserialize_Attitude);

static void BM_Mavlink_Serialize_SysStatus(benchmark::State& state) {
    SysStatus sys;
    sys.onboard_control_sensors_present.value = 10;
    sys.onboard_control_sensors_enabled.value = 10;
    sys.onboard_control_sensors_health.value = 10;
    sys.load.value = 500;
    sys.voltage_battery.value = 11000;
    sys.current_battery.value = 100;
    sys.drop_rate_comm.value = 1;
    sys.errors_comm.value = 2;
    sys.errors_count1.value = 3;
    sys.errors_count2.value = 4;
    sys.errors_count3.value = 5;
    sys.errors_count4.value = 6;
    sys.battery_remaining.value = 80;

    std::array<std::uint8_t, 280> buffer;

    for (auto _ : state) {
        auto res = serialize(sys, 1, 1, 0, buffer);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(buffer);
    }
}
BENCHMARK(BM_Mavlink_Serialize_SysStatus);

static void BM_Mavlink_Deserialize_SysStatus(benchmark::State& state) {
    SysStatus sys;
    sys.onboard_control_sensors_present.value = 10;
    sys.onboard_control_sensors_enabled.value = 10;
    sys.onboard_control_sensors_health.value = 10;
    sys.load.value = 500;
    sys.voltage_battery.value = 11000;
    sys.current_battery.value = 100;
    sys.drop_rate_comm.value = 1;
    sys.errors_comm.value = 2;
    sys.errors_count1.value = 3;
    sys.errors_count2.value = 4;
    sys.errors_count3.value = 5;
    sys.errors_count4.value = 6;
    sys.battery_remaining.value = 80;

    std::array<std::uint8_t, 280> buffer;
    auto res = serialize(sys, 1, 1, 0, buffer);

    MessageView view;
    view.msgid = SysStatus::MessageId;
    view.payload = std::span<const std::uint8_t>(buffer.data() + 10, 31);

    for (auto _ : state) {
        auto res = deserialize<SysStatus>(view);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(BM_Mavlink_Deserialize_SysStatus);

// --- C Library Benchmarks ---

static void BM_MavlinkC_Serialize_Attitude(benchmark::State& state) {
    mavlink_message_t msg;
    std::array<uint8_t, 280> buffer;

    for (auto _ : state) {
        // Pack message into mavlink_message_t
        // mavlink_msg_attitude_pack(system_id, component_id, msg, time_boot_ms, roll, pitch, yaw, rollspeed,
        // pitchspeed, yawspeed)
        mavlink_msg_attitude_pack(1, 1, &msg, 12345678, 1.0f, -1.0f, 0.5f, 0.1f, -0.1f, 0.5f);

        // Serialize to buffer (to be fair comparison with avionicpp which writes to buffer)
        uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
        benchmark::DoNotOptimize(len);
        benchmark::DoNotOptimize(buffer);
    }
}
BENCHMARK(BM_MavlinkC_Serialize_Attitude);

static void BM_MavlinkC_Deserialize_Attitude(benchmark::State& state) {
    mavlink_message_t msg;
    mavlink_msg_attitude_pack(1, 1, &msg, 12345678, 1.0f, -1.0f, 0.5f, 0.1f, -0.1f, 0.5f);

    // Convert to payload buffer for fair comparison?
    // Avionicpp deserializes from span view. C library usually unpacks from mavlink_message_t.
    // The parsing step (framer) produces mavlink_message_t.
    // So usually you do: framer -> mavlink_message_t -> mavlink_msg_attitude_decode

    // We will benchmark the decode step: mavlink_message_t -> mavlink_attitude_t

    mavlink_attitude_t att;

    for (auto _ : state) {
        mavlink_msg_attitude_decode(&msg, &att);
        benchmark::DoNotOptimize(att);
    }
}
BENCHMARK(BM_MavlinkC_Deserialize_Attitude);

static void BM_MavlinkC_Serialize_SysStatus(benchmark::State& state) {
    mavlink_message_t msg;
    std::array<uint8_t, 280> buffer;

    for (auto _ : state) {
        mavlink_msg_sys_status_pack(1, 1, &msg, 10, 10, 10, 500, 11000, 100, 80, 1, 2, 3, 4, 5, 6, 0, 0, 0);

        uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
        benchmark::DoNotOptimize(len);
        benchmark::DoNotOptimize(buffer);
    }
}
BENCHMARK(BM_MavlinkC_Serialize_SysStatus);

static void BM_MavlinkC_Deserialize_SysStatus(benchmark::State& state) {
    mavlink_message_t msg;
    mavlink_msg_sys_status_pack(1, 1, &msg, 10, 10, 10, 500, 11000, 100, 80, 1, 2, 3, 4, 5, 6, 0, 0, 0);

    mavlink_sys_status_t sys;

    for (auto _ : state) {
        mavlink_msg_sys_status_decode(&msg, &sys);
        benchmark::DoNotOptimize(sys);
    }
}
BENCHMARK(BM_MavlinkC_Deserialize_SysStatus);

BENCHMARK_MAIN();
