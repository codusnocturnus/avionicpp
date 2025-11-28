set(target ${PROJECT_NAME})

include(boost-pfr)

add_subdirectory(mavlink)
add_subdirectory(nmea0183)
add_subdirectory(uart)
