#pragma once

#include <deque>

#include "uavpf/nav/exploration_direction.h"
#include "uavpf/nav/nav_grid.h"
#include "uavpf/nav/path_node.h"
#include "uavpf/nav/step_cost.h"

namespace uavpf::experimental
{
	class PathFinder
	{
	public:
		PathFinder(
			const NavGrid& grid,
			NavCell start,
			NavCell end,
			const StepCost& cost);

		bool IsExplorable() const;
		bool IsEnd() const;

		void PeekNext();
		void ExploreNeighbour(ExplorationDirection direction);

		std::vector<NavCell> ReconstructPath() const;

	private:
		bool IsEnd(PathNode* node) const;

		glm::ivec2 ConvertDirectionToCoordOffset(ExplorationDirection direction) const;
		PathNode& GetAssociatedNode(const NavCell& cell);
		std::deque<PathNode*>::const_iterator FindNodeWithLeastCost() const;
		float CalculateHeuristic(const NavCell& cell) const;

		const NavGrid* mGrid = nullptr;
		const StepCost* mCost = nullptr;
		std::deque<PathNode*> mToExplore;
		std::vector<PathNode> mNodePool;
		PathNode* mCurrent = nullptr;
		PathNode* mEnd = nullptr;
	};
}

