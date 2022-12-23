cmake_minimum_required(VERSION 3.16)

project(tuple-polymorphism)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

find_program(CCACHE ccache)
if ((NOT ${CCACHE} MATCHES ".*NOTFOUND")
	AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

find_library(BENCHMARK benchmark REQUIRED)

macro (BUILD_MAIN TYPE MAX_THINGS)
	set(MAIN main.${TYPE}.${MAX_THINGS}.out)

	add_executable(${MAIN}
		main.cpp
		)

	set_target_properties(${MAIN}
		PROPERTIES
			CXX_STANDARD		20
			CXX_STANDARD_REQUIRED	TRUE
			CXX_EXTENSIONS		TRUE
		)

	target_compile_options(${MAIN}
		PRIVATE
			-Wall
			-Wextra
			-Wpedantic
			-O2
		)

	target_compile_definitions(${MAIN}
		PRIVATE
			MAX_THINGS=${MAX_THINGS}
			$<UPPER_CASE:${TYPE}>
		)

	target_link_libraries(${MAIN}
		PRIVATE
			${BENCHMARK}
		)
endmacro()

foreach(TYPE "bench" "virtual" "non_virtual")
	BUILD_MAIN(${TYPE} 0)
	foreach(ORDER_MAGNITUDE RANGE 7)
		BUILD_MAIN(${TYPE} 1e${ORDER_MAGNITUDE})
	endforeach()
endforeach()
