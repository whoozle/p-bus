project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 11)

set(TOOLKIT_NS pbus)
add_subdirectory(src/toolkit)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(IDLCompiler)

set(IDL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/idl)
set(IDL_DST ${CMAKE_CURRENT_BINARY_DIR}/generated/pbus/idl)
set(IDLC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/idl)
file(MAKE_DIRECTORY ${IDL_DST})

set(IDL_FILES
	${IDL_SRC}/Disposable.idl
	${IDL_SRC}/Object.idl
	${IDL_SRC}/Service.idl
	${IDL_SRC}/ServiceLease.idl
	${IDL_SRC}/ServiceManager.idl
)

IDL_GENERATE_SOURCE(
	OUTPUT IDL_SOURCES
	IDLC_PATH ${IDLC_PATH}
	DESTINATION ${IDL_DST}
	INPUT ${IDL_FILES}
)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_BINARY_DIR}/src/toolkit
	${CMAKE_CURRENT_SOURCE_DIR}/src/toolkit/src
	${CMAKE_CURRENT_BINARY_DIR}/generated
)

add_library(idl STATIC ${IDL_SOURCES})


add_library(pbus SHARED
	src/lib/pbus/LocalBus.cpp
	src/lib/pbus/LocalBusConnection.cpp
)

set_target_properties(pbus PROPERTIES VERSION 1.0)
target_link_libraries(pbus toolkit)

add_executable(pbus-init
	src/init/main.cpp
	src/init/pbus/servicemanager/Service.cpp
	src/init/pbus/servicemanager/ServiceRegistry.cpp
)
target_link_libraries(pbus-init pbus)
target_include_directories(pbus-init PRIVATE src/init)

add_executable(pbus-client
	src/client/client.cpp
)
target_link_libraries(pbus-client pbus)
target_include_directories(pbus-client PRIVATE src/client)
