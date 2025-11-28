set(target unittests)

include(CTest)
include(catch2)
include(fakeit)

add_subdirectory(nmea0183)
add_subdirectory(mavlink)
add_subdirectory(uart)

add_executable(${target})
target_sources(${target}
    PRIVATE
    $<TARGET_OBJECTS:nmea0183-tests>
    $<TARGET_OBJECTS:mavlink-tests>
    $<TARGET_OBJECTS:uart-tests>
)

target_link_libraries(${target}
    PRIVATE
    Catch2::Catch2WithMain
    FakeIt::FakeIt-catch
)
set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

include(CTest)
include(Catch)
catch_discover_tests(unittests)
