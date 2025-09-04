#pragma once

#include <string_view>

namespace editor
{
	enum class TerrainDisplayMode
	{
		Monochrome,
		Heightmap,
	};

	constexpr std::string_view ToString(TerrainDisplayMode mode)
	{
		switch (mode)
		{
			case TerrainDisplayMode::Monochrome:
				return "Monochrome";
			case TerrainDisplayMode::Heightmap:
				return "Heightmap";
			default:
				return "";
		}
	}
}

