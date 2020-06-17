include (IDLCompiler)
function(PBUS_BUILD_SERVICE)
	cmake_parse_arguments(_PBS
		"" "NAME;VERSION;IDL_SOURCES_DIR;OUTPUT_DIR" "IDL_SOURCES;SOURCES;DEPENDS" ${ARGN}
	)
	if (NOT _PBS_VERSION)
		set(_PBS_VERSION 1)
	endif()

	message(STATUS "building service ${_PBS_NAME} v${_PBS_VERSION}")

	set(_TARGET "${_PBS_NAME}-${_PBS_VERSION}")

	add_executable(${_TARGET}
		${_PBS_SOURCES}
	)

	if (_PBS_IDL_SOURCES)
		set(_GEN_ROOT "${_PBS_OUTPUT_DIR}/generated")
		set(_IDL_DST "${_GEN_ROOT}/pbus/idl")
		file(MAKE_DIRECTORY ${_IDL_DST})
		message(STATUS "generating idl library idl.${_TARGET}")

		IDL_GENERATE_SOURCE(
			TARGET idl.${_TARGET}
			IDLC_PATH ${IDLC_PATH}
			DESTINATION ${_IDL_DST}
			SOURCES_DIR ${_PBS_IDL_SOURCES_DIR}
			SOURCES ${_PBS_IDL_SOURCES}
		)
		target_link_libraries(${_TARGET} idl.${_TARGET})
	endif()

	target_link_libraries(${_TARGET} pbus)
	target_include_directories(${_TARGET} PRIVATE src/services ${_GEN_ROOT})
	install(TARGETS ${_TARGET} DESTINATION packages/${_TARGET})
	if (_PBS_DEPENDS)
		foreach(_DEP ${_PBS_DEPENDS})
			target_link_libraries(${_TARGET} idl.${_DEP})
		endforeach()
	endif()

	set(_PBS_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/generated/config/${_TARGET}.conf")
	add_custom_command(
		TARGET ${_TARGET}
		BYPRODUCTS ${_PBS_CONFIG}
		POST_BUILD
		COMMAND ${SCAN_DEPS} "${_TARGET}" "${_PBS_CONFIG}" --base="${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "generating config for service ${_TARGET}..."
		DEPENDS ${SCAN_DEPS} ${_TARGET}
	)
	install(FILES ${_PBS_CONFIG} DESTINATION packages/${_TARGET})
endfunction(PBUS_BUILD_SERVICE)
