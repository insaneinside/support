project(Support)
cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
include_directories(include)

add_subdirectory(tests)
add_subdirectory(src)
export(TARGETS support FILE support.cmake)
