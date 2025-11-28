set(target mavlink-core)

add_subdirectory(payloads)

add_library(${target} INTERFACE)

target_sources(${target}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ..
    FILES
    checksum.hpp
    types.hpp
    framer.hpp
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
    Boost::pfr
)

# create combined library and alias
add_library(mavlink INTERFACE)
target_link_libraries(mavlink
    PUBLIC
    INTERFACE
    mavlink-core
    mavlink-payloads
)
add_library(${PROJECT_NAME}::mavlink ALIAS mavlink)
