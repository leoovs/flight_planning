#pragma once

#include "uavpf/nav/exploration_direction.h"
#include "uavpf/nav/nav_cell.h"

namespace uavpf::experimental
{
	struct StepContext
	{
		ExplorationDirection ParentDirection = ExplorationDirection::None;
		ExplorationDirection CurrentDirection = ExplorationDirection::None;
		NavCell Source;
		NavCell Destination;
	};
}

