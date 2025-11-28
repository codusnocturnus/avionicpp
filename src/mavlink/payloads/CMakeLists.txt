set(target mavlink-payloads)

add_library(${target} INTERFACE)
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_sources(${target}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ../..
    FILES
    attitude.hpp
    attitude_quaternion.hpp
    auth_key.hpp
    autopilot_version.hpp
    change_operator_control.hpp
    change_operator_control_ack.hpp
    command_ack.hpp
    command_int.hpp
    command_long.hpp
    global_position_int.hpp
    gps_raw_int.hpp
    gps_status.hpp
    heartbeat.hpp
    local_position_ned.hpp
    param_request_list.hpp
    param_request_read.hpp
    param_set.hpp
    param_value.hpp
    raw_imu.hpp
    raw_pressure.hpp
    rc_channels_raw.hpp
    rc_channels_scaled.hpp
    scaled_imu.hpp
    scaled_pressure.hpp
    sys_status.hpp
    system_time.hpp
    vfr_hud.hpp
)
set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)
target_link_libraries(${target}
    PUBLIC
    INTERFACE
    mavlink
)
