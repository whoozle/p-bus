include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(pbus SHARED
	pbus/Service.cpp
)
target_link_libraries(pbus toolkit)
