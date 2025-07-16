#pragma once

#include <deque>
#include <unordered_map>

#include "uavpf/nav/exploration_direction.h"
#include "uavpf/nav/nav_grid.h"
#include "uavpf/nav/path_graph.h"
#include "uavpf/nav/step_cost.h"

namespace uavpf::experimental
{
	class PathFinder
	{
	public:
		PathFinder(
			const NavGrid& grid,
			const NavCell& start,
			const NavCell& end,
			const StepCost& cost);

		bool IsExplorable() const;
		bool IsEnd() const;

		void PeekNext();
		void ExploreNeighbour(ExplorationDirection direction);

		std::vector<NavCell> ReconstructPath() const;
		std::vector<NavCell> ReconstructAndTruncatePath() const;
		std::vector<NavCell> ReconstructAndTruncatePath2() const;

	private:
		bool IsEnd(PathNode* node) const;

		PathNode& GetAssociatedNode(NavCell cell);
		std::deque<PathNode*>::const_iterator FindNodeWithLeastCost() const;
		float CalculateHeuristic(const NavCell& cell) const;

		const NavGrid* mGrid = nullptr;
		const StepCost* mCost = nullptr;
		std::deque<PathNode*> mToExplore;
		std::unordered_map<size_t, PathNode> mNodePool;
		PathNode* mCurrent = nullptr;
		PathNode* mEnd = nullptr;
	};
}

