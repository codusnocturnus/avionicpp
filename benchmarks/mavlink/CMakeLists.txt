set(target mavlink-benchmarks)

include(google-benchmark)
include(mavlink_c)

add_executable(${target})
target_sources(${target}
    PRIVATE
    main.cpp
)

target_link_libraries(${target}
    PRIVATE
    benchmark::benchmark_main
    mavlink
    mavlink_c
)

set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_link_options(${target}
        PRIVATE
        /PROFILE
    )
endif()
