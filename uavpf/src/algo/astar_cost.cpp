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

	float ContourMatchingCost::CalculateHeuristic(const NavNode* node) const
	{
		glm::ivec2 targetCoord = mGrid->GetCoordinates(mTarget);
		glm::ivec2 currentCoord = mGrid->GetCoordinates(node);

		return glm::length(glm::vec2(targetCoord - currentCoord));
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

	ElevationConservingCost::ElevationConservingCost(float elevation)
		: mTargetElevation(elevation)
	{
	}

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

	float ElevationConservingCost::CalculateHeuristic(const NavNode* node) const
	{
		return mDistanceHeuristic.CalculateHeuristic(node);
	}

	float ElevationConservingCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		float distance = mDistanceHeuristic.CalculateCost(current, neighbour);
		float currentElevation = mGrid->GetElevation(mGrid->GetCoordinates(current));
		float nextElevation = mGrid->GetElevation(mGrid->GetCoordinates(neighbour));

		float slopeCost = nextElevation - currentElevation;
		if (slopeCost > 0.0f)
		{
			slopeCost *= 500.0f;
		}

		float elevationCost = nextElevation;

		return distance + elevationCost + slopeCost;
	}
}

