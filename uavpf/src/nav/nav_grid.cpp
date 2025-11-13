#include "uavpf/nav/nav_grid.h"

#include <cassert>

#include "uavpf/terrain/height_map.h"
#include "uavpf/nav/nav_space.h"

namespace uavpf
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// NavGrid
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	NavGrid::NavGrid()
		: NavGrid({ 1, 1 }, { NavCell{} })
	{}

	NavGrid::NavGrid(NavResolution resolution, std::vector<NavCell> cells)
		: mResolution(std::move(resolution))
		, mCells(std::move(cells))
	{
		assert(resolution.Width * resolution.Depth == mCells.size());
	}

	size_t NavGrid::ToCellIndex(glm::ivec2 navCoords) const
	{
		return navCoords.y * mResolution.Width + navCoords.x;
	}

	bool NavGrid::IsInBounds(glm::ivec2 navCoords) const
	{
		return navCoords.x < mResolution.Width &&
			navCoords.y < mResolution.Depth &&
			navCoords.x >= 0 &&
			navCoords.y >= 0;
	}

	const NavCell& NavGrid::GetCell(glm::ivec2 navCoords) const
	{
		return mCells.at(ToCellIndex(navCoords));
	}

	float NavGrid::GetHeight(glm::ivec2 navCoords) const
	{
		return GetCell(navCoords).RelativeHeight;
	}

	NavResolution NavGrid::GetResolution() const
	{
		return mResolution;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// NavGridBuilder
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	NavGridBuilder& NavGridBuilder::Resize(int32_t width, int32_t depth)
	{
		mResolution = { width, depth };
		mCells.clear();
		mCells.resize(width * depth);
		PopulateNavCoords();
		
		return *this;
	}

	NavGridBuilder& NavGridBuilder::PopulateHeight(const HeightMap& heightMap)
	{
		NavSpace navSpace(mResolution);
		glm::ivec2 heightMapResolution(heightMap.GetWidth(), heightMap.GetDepth());

		for (ptrdiff_t iCell = 0; iCell < mCells.size(); iCell++)
		{
			// TODO: is that ok?
			int32_t navX = iCell % mResolution.Width;
			int32_t navZ = iCell / mResolution.Depth;

			glm::ivec2 navCoord(navX, navZ);
			glm::ivec2 heightMapCoord = navSpace.FromNavCoord(navCoord, heightMapResolution);

			float height = heightMap.GetElevation(heightMapCoord.x, heightMapCoord.y);

			mCells.at(iCell).RelativeHeight = height;
		}

		return *this;
	}

	NavGrid NavGridBuilder::Build()
	{
		return NavGrid(mResolution, std::move(mCells));
	}

	void NavGridBuilder::PopulateNavCoords()
	{
		assert(mCells.size() == mResolution.Width * mResolution.Depth);

		ptrdiff_t iCell = 0;
		for (NavCell& cell : mCells)
		{
			int32_t navX = static_cast<int32_t>(iCell) % mResolution.Width;
			int32_t navZ = static_cast<int32_t>(iCell) / mResolution.Depth;

			cell.Index = iCell++;
			cell.NavCoords = { navX, navZ };
		}
	}
}

