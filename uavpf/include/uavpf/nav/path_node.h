#pragma once

#include "uavpf/nav/step_cost.h"
#include "uavpf/nav/exploration_direction.h"

namespace uavpf::experimental
{
	struct PathNode
	{
		const NavCell* Cell = nullptr;
		PathNode* Parent = nullptr;
		float HeuristicCost = 0.0f;
		float TotalCost = StepCost::cInfinite;
		ExplorationDirection CameFromDirection = ExplorationDirection::None;

		float CalculateScore() const
		{
			return HeuristicCost + TotalCost;
		}
	};
}

