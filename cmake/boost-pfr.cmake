include(FetchContent)
FetchContent_Declare(boost-pfr-fc
    GIT_REPOSITORY https://github.com/boostorg/pfr.git
    GIT_TAG boost-1.89.0
    UPDATE_DISCONNECTED TRUE
    EXCLUDE_FROM_ALL TRUE
)
set(BOOST_USE_MODULES OFF CACHE BOOL "")
set(BUILD_TESTING OFF)
FetchContent_MakeAvailable(boost-pfr-fc)
