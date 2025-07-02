#include "editor/path_planner.h"
#include "uavpf/nav/exploration_direction.h"
#include "uavpf/nav/nav_grid.h"
#include "uavpf/nav/nav_space.h"
#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/step_cost.h"

namespace editor
{
	PathPlanner::PathPlanner(TerrainEditor& terrainEditor)
		: mTerrainEditor(&terrainEditor)
	{
	}

	uavpf::experimental::NavResolution PathPlanner::GetResolution() const
	{
		return mNavGrid.GetResolution();
	}

	void PathPlanner::SetResolution(uavpf::experimental::NavResolution resolution)
	{
		if (mTerrainEditor->IsHeightMapLoaded())
		{
			if (!uavpf::experimental::NavSpace(resolution).IsSingular())
			{
				mNavGrid = uavpf::experimental::NavGridBuilder()
					.Resize(resolution.Width, resolution.Depth)
					.PopulateHeight(mTerrainEditor->GetHeightMap())
					.Build();
				ClampCheckpointNavCoords();
			}
			else
			{
				mNavGrid = {};
			}
		}
	}

	glm::ivec2 PathPlanner::GetNavCoord(CheckpointKind kind) const
	{
		return mCheckpointNavCoords.at(+kind);
	}

	void PathPlanner::SetNavCoord(CheckpointKind kind, glm::ivec2 navCoord)
	{
		mCheckpointNavCoords.at(+kind) = navCoord;
		ClampCheckpointNavCoords();
	}

	glm::vec2 PathPlanner::GetRelativeCoord(CheckpointKind kind) const
	{
		glm::vec2 navCoord = GetNavCoord(kind);
		glm::ivec2 resolution(GetResolution());

		return
		{
			navCoord.x / resolution.x,
			navCoord.y / resolution.y,
		};
	}

	void PathPlanner::SetRelativeCoord(CheckpointKind kind, glm::vec2 relCoord)
	{
		glm::ivec2 resolution(GetResolution());
		SetNavCoord(
			kind,
			{
				resolution.x * relCoord.x,
				resolution.y * relCoord.y,
			}
		);
	}

	auto PathPlanner::QueryCheckpointNavCoords() const -> CheckpointNavCoords
	{
		return mCheckpointNavCoords;
	}

	auto PathPlanner::QueryCheckpointRelativeCoords() const -> CheckpointRelativeCoords
	{
		return
		{
			GetRelativeCoord(CheckpointKind::Start),
			GetRelativeCoord(CheckpointKind::End),
		};
	}

	void PathPlanner::SetCheckpointNavCoords(const CheckpointNavCoords& navCoords)
	{
		mCheckpointNavCoords = navCoords;
	}

	void PathPlanner::SetCheckpointRelativeCoords(const CheckpointRelativeCoords& relCoords)
	{
		SetRelativeCoord(CheckpointKind::Start, relCoords.at(0));
		SetRelativeCoord(CheckpointKind::End, relCoords.at(1));
	}

	glm::ivec2 PathPlanner::NavCoordToHeightMapCoord(glm::ivec2 navCoord) const
	{
		uavpf::experimental::NavSpace navSpace(GetResolution());
		glm::ivec2 heightMapRes = mTerrainEditor->GetHeightMapResolution();
		return navSpace.FromNavCoord(navCoord, heightMapRes);
	}

	void PathPlanner::BeginBuildPath()
	{
		mBuildingPath = true;
	}

	bool PathPlanner::IsBuildingPath()
	{
		return mBuildingPath;
	}

	void PathPlanner::BuildPath()
	{
		uavpf::experimental::ComplexCost costs;
		costs.Add(std::make_unique<uavpf::experimental::ClimbCost>(), 100.0f);
		costs.Add(std::make_unique<uavpf::experimental::DistanceCost>(), 1.0f);

		uavpf::experimental::PathFinder finder(
			mNavGrid,
			mNavGrid.GetCell(mCheckpointNavCoords.at(+CheckpointKind::Start)),
			mNavGrid.GetCell(mCheckpointNavCoords.at(+CheckpointKind::End)),
			costs);

		while (finder.IsExplorable() && mBuildingPath)
		{
			finder.PeekNext();
			if (finder.IsEnd())
			{
				mNavPath = finder.ReconstructPath();
				break;
			}

			for (uavpf::experimental::ExplorationDirection dir
				: uavpf::experimental::EnumerateExplorationDirections())
			{
				finder.ExploreNeighbour(dir);
			}
		}
	}

	void PathPlanner::EndBuildPath()
	{
		mBuildingPath = false;
	}


	const std::vector<uavpf::experimental::NavCell>& PathPlanner::GetPath() const
	{
		return mNavPath;
	}

	float PathPlanner::GetWorldSpaceElevation() const
	{
		return mWorldSpaceElevation;
	}

	void PathPlanner::SetWorldSpaceElevation(float elevation)
	{
		mWorldSpaceElevation = elevation;
	}

	void PathPlanner::ClampCheckpointNavCoords()
	{
		uavpf::experimental::NavResolution res = mNavGrid.GetResolution();

		for (glm::ivec2& navCoord : mCheckpointNavCoords) 
		{
			glm::ivec2 clamped
			{
				glm::clamp(navCoord.x, 0, res.Width),
				glm::clamp(navCoord.y, 0, res.Depth),
			};

			navCoord = clamped;
		}
	}
}

