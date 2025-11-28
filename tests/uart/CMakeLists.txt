set(target uart-tests)

add_library(${target} OBJECT)
target_sources(${target}
    PRIVATE
    uartaccesstests.cpp
    uartcommunicationtests.cpp
    uartconfiguretests.cpp
    stubuarttests.cpp
)
target_link_libraries(${target}
    ${PROJECT_NAME}::uart
    Catch2::Catch2WithMain
    FakeIt::FakeIt-catch
)
set_target_properties(${target}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
)
