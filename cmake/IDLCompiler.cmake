function(IDL_GENERATE_SOURCE)
	cmake_parse_arguments(IDL_GENERATE_SOURCE
		"" "DESTINATION;IDLC_PATH;OUTPUT" "INPUT" ${ARGN}
	)
	message(STATUS "DESTINATION ${IDL_GENERATE_SOURCE_DESTINATION}")
	message(STATUS "IDLC_PATH ${IDL_GENERATE_SOURCE_IDLC_PATH}")
	set(_OUTPUT)
	foreach(_INPUT ${IDL_GENERATE_SOURCE_INPUT})
		get_filename_component(_NAME ${_INPUT} NAME)
		string(LENGTH ${_NAME} _NAME_LEN)
		math(EXPR _NAME_LEN_NOEXT "${_NAME_LEN} - 4")
		string(SUBSTRING ${_NAME} 0 ${_NAME_LEN_NOEXT} _NAME)
		message(STATUS "INPUT NAME ${_NAME}")
		add_custom_command(OUTPUT
			${IDL_GENERATE_SOURCE_DESTINATION}/I${_NAME}.h
			${IDL_GENERATE_SOURCE_DESTINATION}/${_NAME}.h
			${IDL_GENERATE_SOURCE_DESTINATION}/${_NAME}.cpp
			DEPENDS ${_INPUT}
			COMMAND ${IDL_GENERATE_SOURCE_IDLC_PATH}/idlc ${_INPUT} cpp ${IDL_GENERATE_SOURCE_DESTINATION}
			COMMENT "generating sources for ${_NAME}"
		)
		list(APPEND _OUTPUT ${IDL_GENERATE_SOURCE_DESTINATION}/${_NAME}.cpp)
	endforeach()
	set(${IDL_GENERATE_SOURCE_OUTPUT} ${_OUTPUT} PARENT_SCOPE)
endfunction(IDL_GENERATE_SOURCE)
