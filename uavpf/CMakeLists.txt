add_library(uavpf STATIC)

set(UAVPF_ARTIFACT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build-artifacts")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in"
	"${UAVPF_ARTIFACT_DIRECTORY}/config/uavpf/config.h"
)

target_sources(
	uavpf
	PRIVATE
	"src/format_str.cpp"
	"src/version.cpp"

	"src/debug/log_record.cpp"
	"src/debug/logger.cpp"

	"src/id/id_allocator.cpp"
)

target_include_directories(
	uavpf	
	PUBLIC
	"include"
	"${UAVPF_ARTIFACT_DIRECTORY}/config"
	PRIVATE
	"src"
)

set_target_properties(
	uavpf
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
)

unset(UAVPF_ARTIFACT_DIRECTORY)

