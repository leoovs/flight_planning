set(BUILD_SHARED_LIBS OFF)

cpmaddpackage("gh:libsdl-org/SDL#release-3.2.4")

add_executable(editor)

target_sources(
	editor
	PRIVATE
	"src/main.cpp"

	"src/event/event_bus.cpp"
	"src/event/event_dispatcher.cpp"
	"src/event/event_queue.cpp"
	"src/event/event_subscriber.cpp"
)

target_include_directories(
	editor
	PRIVATE
	"src"
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

