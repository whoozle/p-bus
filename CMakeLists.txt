project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 11)

set(TOOLKIT_NS pbus)

add_subdirectory(src/toolkit)
add_subdirectory(src/lib)
