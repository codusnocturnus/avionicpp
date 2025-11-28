set(target nmea0183-core)

add_subdirectory(payloads)

add_library(${target} INTERFACE)

target_sources(${target}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ..
    FILES
    concepts.hpp
    deserializer.hpp
    enumerations.hpp
    framer.hpp
    serializer.hpp
    types.hpp
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

# create the combined library and alias
add_library(nmea0183 INTERFACE)
target_link_libraries(nmea0183
    PUBLIC
    INTERFACE
    nmea0183-core
    nmea0183-payloads
)
add_library(${PROJECT_NAME}::nmea0183 ALIAS nmea0183)
