set(BUILD_SHARED_LIBS OFF)

cpmaddpackage("gh:libsdl-org/SDL#release-3.2.4")

add_executable(editor)

target_sources(
	editor
	PRIVATE
	"src/main.cpp"

	"src/event/event_bus.cpp"
	"src/event/event_dispatcher.cpp"
	"src/event/event_publisher.cpp"
	"src/event/event_queue.cpp"
	"src/event/event_subscriber.cpp"

	"src/graphics_ogl/ogl_facts.cpp"
	"src/graphics_ogl/ogl_graphics_buffer.cpp"
	"src/graphics_ogl/ogl_graphics_context.cpp"

	"src/platform/platform_service.cpp"
	"src/platform_sdl3/sdl3_mouse.cpp"
	"src/platform_sdl3/sdl3_ogl_provider.cpp"
	"src/platform_sdl3/sdl3_platform_service.cpp"
	"src/platform_sdl3/sdl3_window.cpp"
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

