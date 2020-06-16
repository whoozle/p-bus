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
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/deploy/root DESTINATION packages/${_TARGET})
	if (_PBS_DEPENDS)
		foreach(_DEP ${_PBS_DEPENDS})
			target_link_libraries(${_TARGET} idl.${_DEP})
		endforeach()
	endif()
endfunction(PBUS_BUILD_SERVICE)
