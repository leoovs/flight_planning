#include "uavpf/nav/step_cost.h"

#include <glm/glm.hpp>

#include "uavpf/nav/nav_cell.h"

namespace uavpf
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// 
	// DistanceCost 
	// 
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	float DistanceCost::Evaluate(const StepContext& ctx) const
	{
		return glm::length(glm::vec2(ctx.Source.NavCoords - ctx.Destination.NavCoords));
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// 
	// ClimbCost 
	// 
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	float ClimbCost::Evaluate(const StepContext& ctx) const
	{
		return glm::max(ctx.Destination.RelativeHeight - ctx.Source.RelativeHeight, 0.0f);
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// 
	// TurningCost
	// 
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	float TurningCost::Evaluate(const StepContext& ctx) const
	{
		if (ctx.ParentDirection == ExplorationDirection::None)
		{
			return 0.0f;
		}

		auto parentNormalizedDir = glm::normalize(glm::vec2(
			ExplorationDirectionToCoordOffset(ctx.ParentDirection)));
		auto currentNormalizedDir = glm::normalize(glm::vec2(
			ExplorationDirectionToCoordOffset(ctx.CurrentDirection)));

		return 1.0f - glm::dot(parentNormalizedDir, currentNormalizedDir);
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// 
	// ComplexCost 
	// 
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	float ComplexCost::Evaluate(const StepContext& ctx) const
	{
		float totalCost = 0.0f;
		for (const auto& [cost, weight] : mCosts)
		{
			totalCost += weight * cost->Evaluate(ctx);
		}
		return totalCost;
	}

	void ComplexCost::Add(std::unique_ptr<StepCost> cost, float weight)
	{
		mCosts.push_back(std::make_pair(
			std::move(cost),
			weight));
	}

	size_t ComplexCost::GetCostCount() const
	{
		return mCosts.size();
	}

	StepCost& ComplexCost::GetCost(ptrdiff_t iCost)
	{
		return *mCosts.at(iCost).first;
	}

	const StepCost& ComplexCost::GetCost(ptrdiff_t iCost) const
	{
		return *mCosts.at(iCost).first;
	}

	float& ComplexCost::GetWeight(ptrdiff_t iCost)
	{
		return mCosts.at(iCost).second;
	}

	const float& ComplexCost::GetWeight(ptrdiff_t iCost) const
	{
		return mCosts.at(iCost).second;
	}

	void ComplexCost::RemoveCost(ptrdiff_t iCost)
	{
		mCosts.erase(mCosts.begin()+iCost);
	}

	void ComplexCost::ClearCosts()
	{
		mCosts.clear();
	}
}
