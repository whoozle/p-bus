project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 11)

set(TOOLKIT_NS pbus)

add_subdirectory(src/toolkit)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_BINARY_DIR}/src/toolkit
	${CMAKE_CURRENT_SOURCE_DIR}/src/toolkit/src
)

add_library(pbus SHARED
	src/lib/pbus/Service.cpp
	src/lib/pbus/LocalBus.cpp
)
target_link_libraries(pbus toolkit)
