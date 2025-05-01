#include "uavpf/algo/astar_cost.h"

#include <glm/glm.hpp>

#include "uavpf/algo/navgrid.h"

namespace uavpf
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// ContourMatchingHeuristic
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void ContourMatchingCost::SetGrid(const NavGrid* grid)
	{
		mGrid = grid;
	}

	void ContourMatchingCost::SetTarget(const NavNode* target)
	{
		mTarget = target;
	}

	std::string_view ContourMatchingCost::GetName() const
	{
		return "Countor matching";
	}

	float ContourMatchingCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		glm::ivec2 distance = mGrid->GetCoordinates(current) - mGrid->GetCoordinates(neighbour);
		return glm::length(glm::vec2(distance));
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// ElevationConservingHeuristic
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	ElevationConservingCost::ElevationConservingCost() = default;

	void ElevationConservingCost::SetGrid(const NavGrid* grid)
	{
		mDistanceHeuristic.SetGrid(mGrid = grid);
	}

	void ElevationConservingCost::SetTarget(const NavNode* target)
	{
		mDistanceHeuristic.SetTarget(mTarget = target);
	}

	std::string_view ElevationConservingCost::GetName() const
	{
		return "Elevation conservation";
	}

	float ElevationConservingCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		float distanceCost = mDistanceHeuristic.CalculateCost(current, neighbour);
		float currentElevation = mGrid->GetElevation(mGrid->GetCoordinates(current));
		float nextElevation = mGrid->GetElevation(mGrid->GetCoordinates(neighbour));

		float slopeCost = nextElevation - currentElevation;
		float elevationCost = nextElevation;

		return distanceCost + elevationCost + 1000.0f * slopeCost;
	}
}

