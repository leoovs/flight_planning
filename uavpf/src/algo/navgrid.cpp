#include "uavpf/algo/navgrid.h"

#include <cassert>

#include "uavpf/terrain/height_map.h"

namespace uavpf 
{
	NavGrid::NavGrid(NavGridSpecification spec)
		: mSpec(spec)
		, mGraph(spec.Width * spec.Depth)
	{}

	const HeightMap* NavGrid::GetHeightMap() const
	{
		return mHeightMap;
	}

	void NavGrid::SetHeightMap(const HeightMap* heightMap)
	{
		mHeightMap = heightMap;
	}

	const NavGridSpecification& NavGrid::GetSpecification() const
	{
		return mSpec;
	}

	float NavGrid::GetElevation(int32_t navX, int32_t navZ) const
	{
		assert(nullptr != mHeightMap);

		glm::ivec2 imageCoords = ConvertCoordinates({ navX, navZ });

		return mHeightMap->GetElevation(imageCoords.x, imageCoords.y);
	}

	float NavGrid::GetElevation(const glm::ivec2& navCoords) const
	{
		return GetElevation(navCoords.x, navCoords.y);
	}

	NavNode* NavGrid::GetNode(int32_t navX, int32_t navZ)
	{
		if (navX < 0 || navX >= mSpec.Width || navZ < 0 || navZ >= mSpec.Depth)
		{
			return nullptr;
		}

		int32_t index = navZ * mSpec.Width + navX;
		if (index < 0 || index >= mGraph.size())
		{
			return nullptr;
		}
		return &mGraph.at(index);			
	}

	NavNode* NavGrid::GetNode(const glm::ivec2& navCoords)
	{
		return GetNode(navCoords.x, navCoords.y);
	}

	glm::ivec2 NavGrid::GetCoordinates(const NavNode* node) const
	{
		ptrdiff_t index = node - mGraph.data();
		return { index % mSpec.Width, index / mSpec.Width };
	}

	glm::ivec2 NavGrid::ConvertCoordinates(const glm::ivec2& navCoords) const
	{
		assert(navCoords.x < mSpec.Width);
		assert(navCoords.y < mSpec.Depth);

		auto relativeX = float(navCoords.x) / mSpec.Width;
		auto relativeZ = float(navCoords.y) / mSpec.Depth;

		int32_t trueX = relativeX * mHeightMap->GetWidth();
		int32_t trueZ = relativeZ * mHeightMap->GetDepth();

		return { trueX, trueZ };
	}
}
