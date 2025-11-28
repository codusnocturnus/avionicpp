set(target mavlink-tests)

add_library(${target} OBJECT
    test_attitude.cpp
    test_attitude_quaternion.cpp
    test_auth_key.cpp
    test_autopilot_version.cpp
    test_change_operator_control.cpp
    test_change_operator_control_ack.cpp
    test_command_ack.cpp
    test_command_int.cpp
    test_command_long.cpp
    test_global_position_int.cpp
    test_gps_status.cpp
    test_local_position_ned.cpp
    test_heartbeat.cpp
    test_checksum.cpp
    test_gps_raw_int.cpp
    test_param_request_list.cpp
    test_param_request_read.cpp
    test_rc_channels_raw.cpp
    test_rc_channels_scaled.cpp
    test_param_set.cpp
    test_param_value.cpp
    test_raw_imu.cpp
    test_raw_pressure.cpp
    test_scaled_imu.cpp
    test_scaled_pressure.cpp
    test_sys_status.cpp
    test_system_time.cpp
    test_vfr_hud.cpp
)
target_link_libraries(${target}
    PUBLIC
    Catch2::Catch2
    ${PROJECT_NAME}::mavlink
    ${PROJECT_NAME}::mavlink-payloads
)
set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)
