cmake_minimum_required(VERSION 3.20)

project(flight_planning VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(external/get_cpm)

option(FP_BUILD_TESTS OFF "Build flight_planning unit-tests")
set(FP_BUILD_TESTS ON)

add_subdirectory("editor")
add_subdirectory("uavpf")
add_subdirectory("external/glew")

if(FP_BUILD_TESTS)
	enable_testing()
	add_subdirectory("tests/uavpf_tests")
endif()

