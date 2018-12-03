function(IDL_GENERATE_SOURCE)
	cmake_parse_arguments(IDL_GENERATE_SOURCE
		"" "" "INPUT" ${ARGN}
	)
	message(STATUS "INPUT ${IDL_GENERATE_SOURCE_INPUT}")
endfunction(IDL_GENERATE_SOURCE)
