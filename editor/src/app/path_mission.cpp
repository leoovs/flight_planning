#include "app/path_mission.h"

namespace editor
{
	void Path::AddCoordinate(const glm::vec2& coordinate, float elevation)
	{
		mCoordinates.emplace_back(coordinate.x, elevation, coordinate.y);
	}

	void Path::Clear()
	{
		mCoordinates.clear();
	}

	const std::vector<glm::vec3>& Path::GetCoordinates() const
	{
		return mCoordinates;
	}

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

	void PathMission::SetRelativeStart(glm::vec2 relativeStart)
	{
		mRelativeStart = relativeStart;
	}

	glm::vec2 PathMission::GetRelativeStart() const
	{
		return mRelativeStart;
	}


	void PathMission::SetRelativeEnd(glm::vec2 relativeEnd)
	{
		mRelativeEnd = relativeEnd;
	}

	glm::vec2 PathMission::GetRelativeEnd() const
	{
		return mRelativeEnd;
	}

	void PathMission::SetStart(glm::ivec2 start)
	{
		const uavpf::NavGridSpecification& gridSpec = mGrid.GetSpecification();
		mRelativeStart = {
			start.x / float(gridSpec.Width),
			start.y / float(gridSpec.Depth),
		};
	}

	glm::ivec2 PathMission::GetStart() const
	{
		const uavpf::NavGridSpecification& gridSpec = mGrid.GetSpecification();
		return {
			gridSpec.Width * mRelativeStart.x,
			gridSpec.Depth * mRelativeStart.y,
		};
	}

	void PathMission::SetEnd(glm::ivec2 end)
	{
		const uavpf::NavGridSpecification& gridSpec = mGrid.GetSpecification();
		mRelativeEnd = {
			end.x / float(gridSpec.Width),
			end.y / float(gridSpec.Depth),
		};
	}

	glm::ivec2 PathMission::GetEnd() const
	{
		const uavpf::NavGridSpecification& gridSpec = mGrid.GetSpecification();
		return {
			gridSpec.Width * mRelativeEnd.x,
			gridSpec.Depth * mRelativeEnd.y,
		};
	}

	void PathMission::SetMinElevation(float elevation)
	{
		mMinElevation = std::max(elevation, 0.0f);
	}

	float PathMission::GetMinElevation() const
	{
		return mMinElevation;
	}

	void PathMission::SetStatus(PathStatus status)
	{
		mStatus = status;
	}

	PathMission::PathStatus PathMission::GetStatus() const
	{
		return mStatus;
	}

	Path& PathMission::GetPath()
	{
		return mPath;
	}
}

