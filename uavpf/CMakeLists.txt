add_library(uavpf STATIC)

set(UAVPF_ARTIFACT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build-artifacts")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in"
	"${UAVPF_ARTIFACT_DIRECTORY}/config/uavpf/config.h"
)

target_sources(
	uavpf
	PRIVATE
	"src/version.cpp"
)

target_include_directories(
	uavpf	
	PUBLIC
	"include"
	"${UAVPF_ARTIFACT_DIRECTORY}/config"
	PRIVATE
	"src"
)

unset(UAVPF_ARTIFACT_DIRECTORY)

