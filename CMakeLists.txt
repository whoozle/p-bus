project(p-bus)
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 14)
option(PBUS_DEVEL_MODE "enable development mode, lifting some security restriction" OFF)

include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

find_program(SETCAP setcap)
add_definitions(-DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

add_definitions(-D_GNU_SOURCE) #required for GNU extensions such as clone
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)

if (PBUS_DEVEL_MODE)
	set(CMAKE_INSTALL_RPATH "../../lib;./lib;/lib")
	add_definitions(-DPBUS_DEVEL_MODE=1)
endif()

CHECK_SYMBOL_EXISTS(clone sched.h HAVE_CLONE)
CHECK_SYMBOL_EXISTS(unshare sched.h HAVE_UNSHARE)
CHECK_SYMBOL_EXISTS(CLONE_NEWNS sched.h HAVE_CLONE_NEWNS)
CHECK_SYMBOL_EXISTS(CLONE_NEWUTS sched.h HAVE_CLONE_NEWUTS)
CHECK_SYMBOL_EXISTS(CLONE_NEWIPC sched.h HAVE_CLONE_NEWIPC)
CHECK_SYMBOL_EXISTS(CLONE_NEWUSER sched.h HAVE_CLONE_NEWUSER)
CHECK_SYMBOL_EXISTS(CLONE_NEWPID sched.h HAVE_CLONE_NEWPID)
CHECK_SYMBOL_EXISTS(CLONE_NEWNET sched.h HAVE_CLONE_NEWNET)
CHECK_SYMBOL_EXISTS(PR_SET_NO_NEW_PRIVS sys/prctl.h HAVE_PR_SET_NO_NEW_PRIVS)

CHECK_CXX_COMPILER_FLAG(-Wall COMPILER_SUPPORTS_CXX11)
if (COMPILER_SUPPORTS_CXX11)
	list(APPEND CMAKE_CXX_FLAGS -Wall)
endif()

set(TOOLKIT_NS pbus)
add_subdirectory(src/toolkit)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(IDLCompiler)
include(PbusBuildService)

set(IDL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/idl)
set(IDL_DST ${CMAKE_CURRENT_BINARY_DIR}/generated/pbus/idl)
set(IDLC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/tools/idl)
set(SCAN_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/tools/scan-deps)
file(MAKE_DIRECTORY ${IDL_DST})

set(IDL_FILES
	core/Disposable.idl
	core/Object.idl
	system/Service.idl
	system/ServiceLease.idl
	system/ServiceManager.idl
)

IDL_GENERATE_SOURCE(
	TARGET idl.core
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
	src/lib/pbus/idl/core/CoreObject.cpp
	src/lib/pbus/LocalBus.cpp
	src/lib/pbus/LocalBusConnection.cpp
	src/lib/pbus/Session.cpp
)

set_target_properties(pbus PROPERTIES VERSION 1.0)
target_link_libraries(pbus toolkit idl.core)
install(TARGETS pbus RUNTIME DESTINATION lib)

PBUS_BUILD_SERVICE(
	NAME system.ServiceManager
	SOURCES
	src/services/system/servicemanager/main.cpp
	src/services/system/servicemanager/Service.cpp
	src/services/system/servicemanager/ServiceManager.cpp
	src/services/system/servicemanager/ServiceRegistry.cpp
	src/services/system/servicemanager/ServiceConfig.cpp
)

set(SVCMGR_BIN ${CMAKE_INSTALL_PREFIX}/packages/system.ServiceManager-1/system.ServiceManager-1) #fixme: get filename/path from cmake
#sudo couldn't be found using find_program, https://gitlab.kitware.com/cmake/cmake/issues/10468
message(STATUS "sudo ${SETCAP} cap_sys_admin,cap_sys_chroot+ep ${SVCMGR_BIN}")
install(CODE "exec_program(sudo ARGS \"${SETCAP}\" \"cap_sys_admin,cap_sys_chroot+ep\" \"${SVCMGR_BIN}\")")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/deploy/root DESTINATION .)

PBUS_BUILD_SERVICE(
	NAME system.RandomGenerator
	IDL_SOURCES_DIR ${IDL_SRC}
	IDL_SOURCES system/RandomGenerator.idl
	SOURCES src/services/system/randomgenerator/main.cpp
	OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
)

PBUS_BUILD_SERVICE(
	NAME app.pbus.example.random
	SOURCES src/client/client.cpp
	DEPENDS system.RandomGenerator-1
)
