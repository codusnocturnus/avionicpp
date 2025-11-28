include(FetchContent)
FetchContent_Declare(catch2-fc
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.11.0
    UPDATE_DISCONNECTED TRUE
    EXCLUDE_FROM_ALL TRUE
)
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "catch2 config option")
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "catch2 config option")
set(CATCH_DEVELOPMENT_BUILD OFF CACHE BOOL "catch2 config option")
set(CATCH_ENAbLE_REPRODUCIBLE_BUILD ON CACHE BOOL "catch2 config option")

FetchContent_MakeAvailable(catch2-fc)
