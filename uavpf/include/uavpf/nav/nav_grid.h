#pragma once

#include <vector>

#include "uavpf/nav/nav_cell.h"
#include "uavpf/nav/nav_resolution.h"

namespace uavpf
{
	class HeightMap;
}

namespace uavpf
{
	class NavGrid
	{
	public:
		NavGrid();
		NavGrid(NavResolution resolution, std::vector<NavCell> cells);

		size_t ToCellIndex(glm::ivec2 navCoords) const;
		bool IsInBounds(glm::ivec2 navCoords) const;
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

