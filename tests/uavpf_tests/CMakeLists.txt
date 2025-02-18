cpmaddpackage("gh:catchorg/Catch2@3.8.0")

add_executable(uavpf_tests)

target_sources(
	uavpf_tests
	PRIVATE
	"src/test_format_str.cpp"

	"src/debug/test_logger.cpp"

	"src/id/test_id_allocator.cpp"
)

target_link_libraries(
	uavpf_tests
	PRIVATE
	uavpf
	Catch2::Catch2WithMain
)

set_target_properties(
	uavpf_tests
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
)

