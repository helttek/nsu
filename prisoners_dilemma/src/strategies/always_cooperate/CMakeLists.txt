cmake_minimum_required(VERSION 3.27)

project(always_cooperate_strategy)

add_library(always_cooperate SHARED src/always_cooperate_strat.cpp)

set_target_properties(always_cooperate PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/strategies # Output to build/strategies
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/strategies # On Windows, DLLs are also RUNTIME
)