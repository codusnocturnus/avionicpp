set(target uart)

add_library(${target} INTERFACE)
target_sources(${target}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ..
    FILES
    posixuart.hpp
    settings.hpp
    stubuart.hpp
    uart.hpp
    win32uart.hpp
    zephyruart.hpp
)
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

if(WIN32)
    target_sources(${target}
        INTERFACE
        win32uart.cpp
    )
elseif(UNIX)
    target_sources(${target}
        INTERFACE
        posixuart.cpp
    )
endif()

set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
)
