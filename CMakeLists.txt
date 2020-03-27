project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 14)

include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

add_definitions(-D_GNU_SOURCE) #required for GNU extensions such clone
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)

CHECK_SYMBOL_EXISTS(clone sched.h HAVE_CLONE)
CHECK_SYMBOL_EXISTS(unshare sched.h HAVE_UNSHARE)
CHECK_SYMBOL_EXISTS(CLONE_NEWNS sched.h HAVE_CLONE_NEWNS)
CHECK_SYMBOL_EXISTS(CLONE_NEWUTS sched.h HAVE_CLONE_NEWUTS)
CHECK_SYMBOL_EXISTS(CLONE_NEWIPC sched.h HAVE_CLONE_NEWIPC)
CHECK_SYMBOL_EXISTS(CLONE_NEWUSER sched.h HAVE_CLONE_NEWUSER)
CHECK_SYMBOL_EXISTS(CLONE_NEWPID sched.h HAVE_CLONE_NEWPID)
CHECK_SYMBOL_EXISTS(CLONE_NEWNET sched.h HAVE_CLONE_NEWNET)

CHECK_CXX_COMPILER_FLAG(-Wall COMPILER_SUPPORTS_CXX11)
if (COMPILER_SUPPORTS_CXX11)
	list(APPEND CMAKE_CXX_FLAGS -Wall)
endif()

set(TOOLKIT_NS pbus)
add_subdirectory(src/toolkit)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(IDLCompiler)

set(IDL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/idl)
set(IDL_DST ${CMAKE_CURRENT_BINARY_DIR}/generated/pbus/idl)
set(IDLC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/idl)
file(MAKE_DIRECTORY ${IDL_DST})

set(IDL_FILES
	core/Disposable.idl
	core/Object.idl
	system/Service.idl
	system/ServiceLease.idl
	system/ServiceManager.idl
	system/RandomGenerator.idl
)

IDL_GENERATE_SOURCE(
	OUTPUT IDL_SOURCES
	IDLC_PATH ${IDLC_PATH}
	DESTINATION ${IDL_DST}
	SOURCES_DIR ${IDL_SRC}
	SOURCES ${IDL_FILES}
)


include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_BINARY_DIR}/src/toolkit
	${CMAKE_CURRENT_SOURCE_DIR}/src/toolkit/src
	${CMAKE_CURRENT_BINARY_DIR}/generated
)

add_library(pbus SHARED
	src/lib/pbus/ClassId.cpp
	src/lib/pbus/ObjectId.cpp
	src/lib/pbus/Application.cpp
	src/lib/pbus/idl/core/ICoreObject.cpp
	src/lib/pbus/LocalBus.cpp
	src/lib/pbus/LocalBusConnection.cpp
	src/lib/pbus/Session.cpp
	${IDL_SOURCES}
)

set_target_properties(pbus PROPERTIES VERSION 1.0)
target_link_libraries(pbus toolkit)

add_executable(system.ServiceManager
	src/services/system/servicemanager/main.cpp
	src/services/system/servicemanager/Service.cpp
	src/services/system/servicemanager/ServiceRegistry.cpp
)
target_link_libraries(system.ServiceManager pbus)
target_include_directories(system.ServiceManager PRIVATE src/services)

add_executable(system.RandomGenerator
	src/services/system/randomgenerator/main.cpp
)
target_link_libraries(system.RandomGenerator pbus)
target_include_directories(system.RandomGenerator PRIVATE src/services)


add_executable(app.pbus.example.random
	src/client/client.cpp
)
target_link_libraries(app.pbus.example.random pbus)
target_include_directories(app.pbus.example.random PRIVATE src/client)
