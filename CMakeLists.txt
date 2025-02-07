cmake_minimum_required(VERSION 3.20)

project(flight_planning VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(external/get_cpm)

add_subdirectory("editor")
add_subdirectory("uavpf")
add_subdirectory("external/glew")

