#pragma once

#include <vector>

#include "uavpf/nav/nav_cell.h"
#include "uavpf/nav/nav_resolution.h"

namespace uavpf
{
	class HeightMap;
}

namespace uavpf::experimental
{
	class NavGrid
	{
	public:
		NavGrid(NavResolution resolution, std::vector<NavCell> cells);

		const NavCell& GetCell(glm::ivec2 navCoords) const;
		float GetHeight(glm::ivec2 navCoords) const;
		NavResolution GetResolution() const;

	private:
		NavResolution mResolution;	
		std::vector<NavCell> mCells;
	};

	class NavGridBuilder
	{
	public:
		NavGridBuilder& Resize(int32_t width, int32_t depth);
		NavGridBuilder& PopulateHeight(const HeightMap& heightMap);
		NavGrid Build();

	private:
		void PopulateNavCoords();

		NavResolution mResolution;
		std::vector<NavCell> mCells;
	};
}

