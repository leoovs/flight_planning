set(BUILD_SHARED_LIBS OFF)

cpmaddpackage("gh:libsdl-org/SDL#release-3.2.4")

add_executable(editor)

target_sources(
	editor
	PRIVATE
	"src/main.cpp"

	"src/app/app_context.cpp"
	"src/app/app_service.cpp"
	"src/app/asset_storage.cpp"
	"src/app/editor_app.cpp"
	"src/app/editor_state.cpp"
	"src/app/path_mission.cpp"
	"src/app/tasks.cpp"
	"src/app/terrain_scaler.cpp"

	"src/event/event_bus.cpp"
	"src/event/event_dispatcher.cpp"
	"src/event/event_publisher.cpp"
	"src/event/event_queue.cpp"
	"src/event/event_subscriber.cpp"

	"src/graphics/graphics_buffer_slots.cpp"
	"src/graphics/graphics_format.cpp"

	"src/graphics_ogl/ogl_facts.cpp"
	"src/graphics_ogl/ogl_framebuffer.cpp"
	"src/graphics_ogl/ogl_graphics_buffer.cpp"
	"src/graphics_ogl/ogl_graphics_context.cpp"
	"src/graphics_ogl/ogl_graphics_debug_watch.cpp"
	"src/graphics_ogl/ogl_imgui_graphics_backend.cpp"
	"src/graphics_ogl/ogl_shader.cpp"
	"src/graphics_ogl/ogl_shader_compilation.cpp"
	"src/graphics_ogl/ogl_shader_compiler.cpp"
	"src/graphics_ogl/ogl_shader_pipeline.cpp"
	"src/graphics_ogl/ogl_texture_2d.cpp"
	"src/graphics_ogl/ogl_vertex_input.cpp"

	"src/platform/platform_service.cpp"

	"src/platform_sdl3/sdl3_imgui_platform_backend.cpp"
	"src/platform_sdl3/sdl3_keyboard.cpp"
	"src/platform_sdl3/sdl3_mouse.cpp"
	"src/platform_sdl3/sdl3_ogl_provider.cpp"
	"src/platform_sdl3/sdl3_platform_service.cpp"
	"src/platform_sdl3/sdl3_window.cpp"

	"src/rendering/camera.cpp"
	"src/rendering/frame_timer.cpp"
	"src/rendering/overlay_renderer.cpp"
	"src/rendering/terrain_render_mesh.cpp"
	"src/rendering/terrain_renderer.cpp"

	"src/tasking/task.cpp"
	"src/tasking/task_scheduler.cpp"
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
	imgui
	tinyfiledialogs	
	SDL3::SDL3-static
)

set_target_properties(
	editor
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
)

