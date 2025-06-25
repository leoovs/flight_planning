#include "uavpf/nav/step_cost.h"

#include <glm/glm.hpp>

#include "uavpf/nav/nav_cell.h"

namespace uavpf::experimental
{
	float DistanceCost::Evaluate(const NavCell& src, const NavCell& dst) const
	{
		return glm::distance(glm::vec2(src.NavCoords), glm::vec2(dst.NavCoords));
	}

	float ClimbCost::Evaluate(const NavCell& src, const NavCell& dst) const
	{
		return glm::max(dst.RelativeHeight - src.RelativeHeight, 0.0f);
	}

	float ComplexCost::Evaluate(const NavCell& src, const NavCell& dst) const
	{
		float totalCost = 0.0f;
		for (const auto& [cost, weight] : mCosts)
		{
			totalCost += weight * cost->Evaluate(src, dst);
		}
		return totalCost;
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
