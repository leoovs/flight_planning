#include "editor/path_planner.h"
#include "uavpf/nav/nav_grid.h"
#include "uavpf/nav/nav_space.h"

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

	glm::ivec2 PathPlanner::NavCoordToHeightMapCoord(glm::ivec2 navCoord) const
	{
		uavpf::experimental::NavSpace navSpace(GetResolution());
		glm::ivec2 heightMapRes = mTerrainEditor->GetHeightMapResolution();
		return navSpace.FromNavCoord(navCoord, heightMapRes);
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

