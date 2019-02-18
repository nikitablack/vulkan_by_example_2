cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_library(tl INTERFACE)

target_sources(tl INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tl/expected.hpp
)

target_include_directories(tl SYSTEM INTERFACE include)