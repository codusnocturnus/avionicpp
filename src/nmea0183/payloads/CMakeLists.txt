set(target nmea0183-payloads)

add_library(${target} INTERFACE)

target_sources(${target}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ../..
    FILES
    dtm.hpp
    gbs.hpp
    gga.hpp
    gll.hpp
    gns.hpp
    gsa.hpp
    gst.hpp
    hdt.hpp
    rmc.hpp
    rot.hpp
    vtg.hpp
    zda.hpp
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
    nmea0183-core
)
