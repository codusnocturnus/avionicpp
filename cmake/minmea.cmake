include(FetchContent)

FetchContent_Declare(
  minmea-fc
  GIT_REPOSITORY https://github.com/kosma/minmea.git
  GIT_TAG master
  PATCH_COMMAND git apply --inaccurate-eof ${CMAKE_SOURCE_DIR}/patches/minmea.patch
  UPDATE_DISCONNECTED TRUE
  EXCLUDE_FROM_ALL TRUE
)
set(MINMEA_ENABLE_TESTING OFF CACHE BOOL "")
FetchContent_MakeAvailable(minmea-fc)
