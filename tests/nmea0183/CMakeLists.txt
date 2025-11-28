set(target nmea0183-tests)

add_library(${target} OBJECT
    test_deserializer.cpp
    test_dtm.cpp
    test_framer.cpp
    test_gbs.cpp
    test_gga.cpp
    test_gll.cpp
    test_gns.cpp
    test_gsa.cpp
    test_gst.cpp
    test_hdt.cpp
    test_rmc.cpp
    test_rot.cpp
    test_serializer.cpp
    test_utilities.cpp
    test_vtg.cpp
    test_zda.cpp
)
target_link_libraries(${target}
    PUBLIC
    Catch2::Catch2
    ${PROJECT_NAME}::nmea0183
)
set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)
