add_library(imgui STATIC)

target_sources(
	imgui
	PRIVATE

	"src/imgui/imgui.cpp"
	"src/imgui/imgui_demo.cpp"
	"src/imgui/imgui_draw.cpp"
	"src/imgui/imgui_impl_opengl3.cpp"
	"src/imgui/imgui_impl_sdl3.cpp"
	"src/imgui/imgui_tables.cpp"
	"src/imgui/imgui_widgets.cpp"
)

target_include_directories(
	imgui
	PUBLIC
	"include"
	"include/imgui"
)

target_link_libraries(
	imgui
	PRIVATE
	SDL3::SDL3-static
)

