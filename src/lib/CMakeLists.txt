include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../toolkit
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/toolkit/src
)

add_library(pbus SHARED
	pbus/Service.cpp
	pbus/LocalBus.cpp
)
target_link_libraries(pbus toolkit)
