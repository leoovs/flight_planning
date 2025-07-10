set(BUILD_SHARED_LIBS OFF)

# cpmaddpackage("gl:libtiff/libtiff@4.7.0")

cpmaddpackage(
	NAME libtiff
	GITLAB_REPOSITORY libtiff/libtiff
	VERSION 4.7.0
	OPTIONS "CMAKE_POLICY_VERSION_MINIMUM 3.5"
)
cpmaddpackage("gh:g-truc/glm#1.0.1")
cpmaddpackage("gh:leoovs/unipp#dev-eachindex")

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

	"src/algo/astar_algorithm.cpp"
	"src/algo/astar_cost.cpp"
	"src/algo/navgrid.cpp"

	"src/debug/concise_log_formatter.cpp"
	"src/debug/console_log_emitter.cpp"
	"src/debug/log_record.cpp"
	"src/debug/logger.cpp"
	"src/debug/logger_provider.cpp"

	"src/id/id_allocator.cpp"

	"src/image/image_grayscale.cpp"
	"src/image/tiff_image.cpp"
	"src/image/tiff_loader.cpp"
	"src/image/tiff_tag_accessor_impl.cpp"

	"src/nav/nav_grid.cpp"
	"src/nav/nav_space.cpp"
	"src/nav/path_finder.cpp"
	"src/nav/step_cost.cpp"

	"src/terrain/height_map.cpp"
	"src/terrain/height_map_builder.cpp"
	"src/terrain/terrain_mesh.cpp"
	"src/terrain/terrain_mesh_builder.cpp"
)

target_include_directories(
	uavpf	
	PUBLIC
	"include"
	"${UAVPF_ARTIFACT_DIRECTORY}/config"
	PRIVATE
	"src"
)

target_link_libraries(
	uavpf
	PUBLIC
	glm::glm
	PRIVATE
	TIFF::tiff
	unipp::unipp
)

target_compile_definitions(
	uavpf
	PUBLIC
	"GLM_ENABLE_EXPERIMENTAL"
)

set_target_properties(
	uavpf
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE
)

unset(UAVPF_ARTIFACT_DIRECTORY)

