project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 11)

set(TOOLKIT_NS pbus)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(IDLCompiler)

set(IDL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/idl)
IDL_GENERATE_SOURCE(INPUT ${IDL_SRC}/Disposable.idl ${IDL_SRC}/Object.idl ${IDL_SRC}/ServiceLease.idl ${IDL_SRC}/ServiceManager.idl)

add_subdirectory(src/toolkit)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_BINARY_DIR}/src/toolkit
	${CMAKE_CURRENT_SOURCE_DIR}/src/toolkit/src
)

add_library(pbus SHARED
	src/lib/pbus/Service.cpp
	src/lib/pbus/LocalBus.cpp
	src/lib/pbus/ServiceRegistry.cpp
)
set_target_properties(pbus PROPERTIES VERSION 1.0)
target_link_libraries(pbus toolkit)

add_executable(pbus-init src/init/main.cpp)
target_link_libraries(pbus-init pbus)
