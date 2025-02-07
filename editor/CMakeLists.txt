set(BUILD_SHARED_LIBS OFF)

cpmaddpackage("gh:libsdl-org/SDL#release-3.2.4")

add_executable(editor)

target_sources(
	editor
	PRIVATE
	"src/main.cpp"
)

target_link_libraries(
	editor
	PRIVATE
	uavpf
	glew
	SDL3::SDL3-static
)

set_target_properties(
	editor
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
)

