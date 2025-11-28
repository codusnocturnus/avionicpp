cmake_minimum_required(VERSION 3.31)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(avionicpp
    VERSION 2025.11.28
    DESCRIPTION "Avionicpp - a library for communicating with common avionics devices and processing their data"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(AVIONICPP_BUILD_TESTS "Build tests for Avionicpp" ON)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

add_subdirectory(benchmarks)
