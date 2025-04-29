add_library(tinyfiledialogs STATIC)

target_sources(
	tinyfiledialogs
	PRIVATE
	"src/tinyfiledialogs.c"
)

target_include_directories(
	tinyfiledialogs
	PUBLIC
	"include"
)
