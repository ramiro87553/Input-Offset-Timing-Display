
cmake_minimum_required(VERSION 3.25)

project(InputOffsetDisplay VERSION 1.0.0)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable is not set")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_BINARY_DIR}/geode-sdk)

geode_target(InputOffsetDisplay)

target_sources(InputOffsetDisplay PRIVATE
    src/main.cpp
)
