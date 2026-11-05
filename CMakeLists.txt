
cmake_minimum_required(VERSION 3.25)

project(InputOffsetDisplay VERSION 1.0.0)

find_package(Geode REQUIRED)

add_library(InputOffsetDisplay SHARED
    src/main.cpp
)

setup_geode_mod(InputOffsetDisplay)
