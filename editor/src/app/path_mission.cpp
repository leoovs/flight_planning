#include "app/path_mission.h"

namespace editor
{
	void PathMission::SetHeightMap(uavpf::HeightMap heightMap)
	{
		mHeightMap = std::move(heightMap);
		mGrid.SetHeightMap(&mHeightMap);
	}

	void PathMission::SpecifyGrid(uavpf::NavGridSpecification spec)
	{
		mGrid = uavpf::NavGrid(spec);
		mGrid.SetHeightMap(&mHeightMap);
	}

	const uavpf::NavGrid& PathMission::GetNavGrid() const
	{
		return mGrid;
	}

	void PathMission::SetStart(glm::vec2 start)
	{
		mStart = start;
	}

	glm::vec2 PathMission::GetStart() const
	{
		return mStart;
	}


	void PathMission::SetEnd(glm::vec2 end)
	{
		mEnd = end;
	}

	glm::vec2 PathMission::GetEnd() const
	{
		return mEnd;
	}
}

