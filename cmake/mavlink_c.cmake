include(FetchContent)

FetchContent_Declare(
    mavlink_c_library_v2
    GIT_REPOSITORY https://github.com/mavlink/c_library_v2.git
    GIT_TAG        master
    PATCH_COMMAND ${CMAKE_COMMAND} -E touch CMakeLists.txt
    UPDATE_DISCONNECTED TRUE
    EXCLUDE_FROM_ALL TRUE
)
FetchContent_MakeAvailable(mavlink_c_library_v2)

# Create an interface library for the headers
if(NOT TARGET mavlink_c)
    add_library(mavlink_c INTERFACE)
    target_include_directories(mavlink_c INTERFACE ${mavlink_c_library_v2_SOURCE_DIR})
endif()
