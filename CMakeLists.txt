cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(teapot LANGUAGES CXX)

add_subdirectory(src/third_party/eigen)
add_subdirectory(src/third_party/glfw-3.2.1)
add_subdirectory(src/third_party/tl)
add_subdirectory(src/teapot)
