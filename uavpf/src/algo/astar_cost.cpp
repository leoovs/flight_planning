#include "uavpf/algo/astar_cost.h"

#include <glm/glm.hpp>

#include "uavpf/algo/navgrid.h"

namespace uavpf
{
	std::string_view GetAStarCostKindString(AStarCostKind kind)
	{
		switch (kind)
		{
			case AStarCostKind::Unknown:
				return "Unknown";
			case AStarCostKind::Distance:
				return "Distance";
			case AStarCostKind::Slope:
				return "Slope";
			case AStarCostKind::Notam:
				return "Notam";
			case AStarCostKind::Collection:
				return "Collection";
			default:
				assert(false && "Unknwon AStarCostKind enum value");
				return "";
		}
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// DistanceCost
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void DistanceCost::SetGrid(const NavGrid* grid)
	{
		mGrid = grid;
	}

	AStarCostKind DistanceCost::GetKind() const
	{
		return AStarCostKind::Distance;
	}

	float DistanceCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		glm::vec2 currentCoord(mGrid->GetCoordinates(current));
		glm::vec2 neighbourCoord(mGrid->GetCoordinates(neighbour));

		return glm::length(currentCoord - neighbourCoord);
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// SlopeCost 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void SlopeCost::SetGrid(const NavGrid* grid)
	{
		mGrid = grid;
	}

	AStarCostKind SlopeCost::GetKind() const
	{
		return AStarCostKind::Slope;
	}

	float SlopeCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		glm::ivec2 currentCoord = mGrid->GetCoordinates(current);
		glm::ivec2 neighbourCoord = mGrid->GetCoordinates(neighbour);

		float currentHeight = mGrid->GetElevation(currentCoord);
		float neighbourHeight = mGrid->GetElevation(neighbourCoord);

		return std::max(neighbourHeight - currentHeight, 0.0f);
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// NotamCost 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void NotamCost::SetGrid(const NavGrid* grid)
	{
		mGrid = grid;
	}

	AStarCostKind NotamCost::GetKind() const
	{
		return AStarCostKind::Notam;
	}

	float NotamCost::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		auto [width, height] = mGrid->GetSpecification();
		glm::ivec2 neighbourCoord = mGrid->GetCoordinates(neighbour);

		float x = neighbourCoord.x - (mCenter.x * width);
		float y = neighbourCoord.y - (mCenter.y * height);
		float a = (mRadius * width);	
		float b = (mRadius * height);	

		bool isInsideEllipse = glm::pow(x, 2.0f)/glm::pow(a, 2.0f) + glm::pow(y, 2.0f)/glm::pow(b, 2) < 1.0f;

		return isInsideEllipse
			? NavNode::cInfiniteCost 
			: 0.0f;
	}

	glm::vec2& NotamCost::GetCenter()
	{
		return mCenter;
	}

	glm::ivec2 NotamCost::GetNavSpaceCenter()
	{
		return {
			mCenter.x * mGrid->GetSpecification().Width,
			mCenter.y * mGrid->GetSpecification().Depth,
		};
	}

	float& NotamCost::GetRadius()
	{
		return mRadius;
	}

	int32_t NotamCost::GetRadiusA() const
	{
		return mRadius * mGrid->GetSpecification().Width;
	}

	int32_t NotamCost::GetRadiusB() const
	{
		return mRadius * mGrid->GetSpecification().Depth;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// CostCollection 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void CostCollection::SetGrid(const NavGrid* grid)
	{
		mGrid = grid;
		for (std::unique_ptr<AStarCost>& cost : mCosts)
		{
			cost->SetGrid(grid);
		}
	}

	AStarCostKind CostCollection::GetKind() const
	{
		return AStarCostKind::Collection;
	}

	float CostCollection::CalculateCost(const NavNode* current, const NavNode* neighbour) const
	{
		float totalWeightedCost = 0.0f;

		for (ptrdiff_t iCost = 0; iCost < mCosts.size(); iCost++)
		{
			float cost = mCosts.at(iCost)->CalculateCost(current, neighbour);
			float weightedCost = mWeights.at(iCost) * cost;
			totalWeightedCost += weightedCost;
		}

		return totalWeightedCost;
	}

	size_t CostCollection::GetSize() const
	{
		return mCosts.size();
	}

	void CostCollection::Erase(ptrdiff_t iCost)
	{
		mCosts.erase(mCosts.begin() + iCost);
		mWeights.erase(mWeights.begin() + iCost);
	}

	void CostCollection::Clear()
	{
		mCosts.clear();
		mWeights.clear();
	}

	AStarCost* CostCollection::GetCost(ptrdiff_t iCost)
	{
		return mCosts.at(iCost).get();
	}

	float& CostCollection::GetWeight(ptrdiff_t iCost)
	{
		return mWeights.at(iCost);
	}

	void CostCollection::PushBack(std::unique_ptr<AStarCost> cost, float weight)
	{
		cost->SetGrid(mGrid);
		mCosts.push_back(std::move(cost));
		mWeights.push_back(weight);
	}
}
