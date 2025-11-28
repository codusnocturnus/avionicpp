include(FetchContent)
FetchContent_Declare(fakeit-fc
    GIT_REPOSITORY https://github.com/eranpeer/FakeIt.git
    GIT_TAG 2.5.0
    UPDATE_DISCONNECTED TRUE
    EXCLUDE_FROM_ALL TRUE
)
set(ENABLE_TESTING OFF CACHE BOOL "")
set(OVERRIDE_CXX_STANDARD_FOR_TESTS "" CACHE STRING "")
set(ENABLE_SANITIZERS_IN_TESTS OFF CACHE BOOL "")
set(ENABLE_COVERAGE OFF CACHE BOOL "")
FetchContent_MakeAvailable(fakeit-fc)
