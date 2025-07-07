#include "editor/path_planner.h"
#include "uavpf/nav/exploration_direction.h"
#include "uavpf/nav/nav_grid.h"
#include "uavpf/nav/nav_resolution.h"
#include "uavpf/nav/nav_space.h"
#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/step_cost.h"

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// NotamCost
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	NotamCost::NotamCost(glm::vec2 navCoord, glm::vec2 ellipse)
		: mNotamNavCoord(navCoord)
		, mNotamNavEllipse(ellipse)
	{}

	float NotamCost::Evaluate(const uavpf::experimental::StepContext& ctx) const
	{
		float x = ctx.Destination.NavCoords.x;
		float y = ctx.Destination.NavCoords.y;

		float h = mNotamNavCoord.x;
		float k = mNotamNavCoord.y;

		float a = mNotamNavEllipse.x;
		float b = mNotamNavEllipse.y;

		// Ellipse equation
		float p = glm::pow(x - h, 2) / glm::pow(a, 2) + glm::pow(y - k, 2) / glm::pow(b, 2);

		return p < 1.0f
			? cInfinite
			: 0.0f;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// PathPlanner
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	PathPlanner::PathPlanner(const TerrainEditor& terrainEditor, const NavNetwork& navNetwork)
		: mTerrainEditor(&terrainEditor)
		, mNavNetwork(&navNetwork)
	{}

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
		glm::ivec2 resolution(mNavNetwork->GetResolution());

		return
		{
			navCoord.x / resolution.x,
			navCoord.y / resolution.y,
		};
	}

	void PathPlanner::SetRelativeCoord(CheckpointKind kind, glm::vec2 relCoord)
	{
		glm::ivec2 resolution(mNavNetwork->GetResolution());
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

	float PathPlanner::GetWeight(PathPlannerWeight name) const
	{
		return mWeights.at(+name);
	}

	void PathPlanner::SetWeight(PathPlannerWeight name, float weight)
	{
		mWeights.at(+name) = weight;
	}

	glm::ivec2 PathPlanner::NavCoordToHeightMapCoord(glm::ivec2 navCoord) const
	{
		uavpf::experimental::NavSpace navSpace(mNavNetwork->GetResolution());
		glm::ivec2 heightMapRes = mTerrainEditor->GetHeightMapResolution();
		return navSpace.FromNavCoord(navCoord, heightMapRes);
	}

	void PathPlanner::BeginBuildPath()
	{
		mBuildingPath = true;
	}

	bool PathPlanner::IsBuildingPath() const
	{
		return mBuildingPath;
	}

	void PathPlanner::BuildPath()
	{
		uavpf::experimental::ComplexCost costs;

		float distanceWeight = mWeights.at(+PathPlannerWeight::Distance);
		float climbWeight = mWeights.at(+PathPlannerWeight::Climb);
		float turningWeight = mWeights.at(+PathPlannerWeight::Turning);

		costs.Add(std::make_unique<uavpf::experimental::DistanceCost>(), distanceWeight);
		costs.Add(std::make_unique<uavpf::experimental::ClimbCost>(), climbWeight);
		costs.Add(std::make_unique<uavpf::experimental::TurningCost>(), turningWeight);

		PopulateNotamCosts(costs);

		uavpf::experimental::NavGrid grid = mNavNetwork->GetGrid();
		uavpf::experimental::PathFinder finder(
			grid,
			grid.GetCell(mCheckpointNavCoords.at(+CheckpointKind::Start)),
			grid.GetCell(mCheckpointNavCoords.at(+CheckpointKind::End)),
			costs);

		while (finder.IsExplorable() && mBuildingPath)
		{
			finder.PeekNext();
			if (finder.IsEnd())
			{
				mNavPath = finder.ReconstructPath();
				return;
			}

			for (uavpf::experimental::ExplorationDirection dir
				: uavpf::experimental::EnumerateExplorationDirections())
			{
				finder.ExploreNeighbour(dir);
			}
		}

		mNavPath.clear();
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
		uavpf::experimental::NavResolution res = mNavNetwork->GetResolution();

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

	void PathPlanner::PopulateNotamCosts(uavpf::experimental::ComplexCost& costs)
	{
		uavpf::experimental::NavResolution res = mNavNetwork->GetResolution();
		size_t notamCount = mNavNetwork->GetNotamCount();

		for (size_t iNotam = 0; iNotam < notamCount; iNotam++)
		{
			Notam notam = mNavNetwork->GetNotam(iNotam);

			glm::vec2 navCoords = notam.NavCoord;
			glm::vec2 notamEllipse
			{
				notam.RelativeRadius * res.Width,
				notam.RelativeRadius * res.Depth,
			};

			costs.Add(std::make_unique<NotamCost>(navCoords, notamEllipse));
		}
	}
}

