cmake_minimum_required(VERSION 3.25)
project(InputOffsetDisplay VERSION 1.0.0)

find_package(Geode REQUIRED)

geode_target(InputOffsetDisplay)

target_sources(InputOffsetDisplay PRIVATE
    src/main.cpp
)
