#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/exploration_direction.h"

#include <algorithm>

namespace uavpf::experimental
{
	PathFinder::PathFinder(
		const NavGrid& grid,
		const NavCell& start,
		const NavCell& end,
		const StepCost& cost)
		: mGrid(&grid)
		, mEnd(nullptr)
		, mCost(&cost)
	{
		mEnd = &GetAssociatedNode(end);	

		PathNode& startNode = GetAssociatedNode(start);
		startNode.TotalCost = 0.0f;
		startNode.HeuristicCost = CalculateHeuristic(start);
		mToExplore.push_back(&startNode);
	}

	bool PathFinder::IsExplorable() const
	{
		return !mToExplore.empty();
	}

	bool PathFinder::IsEnd() const
	{
		return IsEnd(mCurrent);
	}

	void PathFinder::PeekNext()
	{
		auto it = FindNodeWithLeastCost();
		mCurrent = *it;

		if (IsEnd())
		{
			return;
		}

		mToExplore.erase(it);
	}

	void PathFinder::ExploreNeighbour(ExplorationDirection direction)
	{
		glm::ivec2 neighbourCoords = mCurrent->Cell->NavCoords
			+ ConvertDirectionToCoordOffset(direction);
		if (!mGrid->IsInBounds(neighbourCoords))
		{
			return;
		}

		mCurrent->NextDirection = direction;
		PathNode& neighbour = GetAssociatedNode(mGrid->GetCell(neighbourCoords));

		float tentativeScore = mCurrent->TotalCost + mCost->Evaluate(
			*mCurrent->Cell,
			*neighbour.Cell);

		if (tentativeScore < neighbour.TotalCost)
		{
			neighbour.Parent = mCurrent;
			neighbour.TotalCost = tentativeScore;
			neighbour.HeuristicCost = CalculateHeuristic(*neighbour.Cell);

			auto it = std::find(mToExplore.begin(), mToExplore.end(), &neighbour);	
			if (mToExplore.end() == it)
			{
				mToExplore.push_back(&neighbour);
			}
		}
	}

	std::vector<NavCell> PathFinder::ReconstructPath() const
	{
		// TODO: fix ignore of the flat surfaces...

		std::vector<NavCell> path;

		if (mCurrent == nullptr)
			return path;

		// Collect all nodes in reverse order (child → parent)
		std::vector<PathNode*> nodes;
		PathNode* current = mCurrent;
		while (current != nullptr)
		{
			nodes.push_back(current);
			current = current->Parent;
		}

		if (nodes.empty())
			return path;

		// The first node is always included
		path.push_back(*nodes.back()->Cell);
		if (nodes.size() == 1)
			return path;  // Only one node, nothing to merge

		ExplorationDirection lastDir = nodes.back()->NextDirection;
		float lastHeight = nodes.back()->Cell->RelativeHeight;
		bool isIncreasing = false;
		bool isDecreasing = false;
		bool trendInitialized = false;

		// Iterate from second-to-last node to the start (parent → child)
		for (int i = nodes.size() - 2; i >= 0; --i)
		{
			PathNode* node = nodes[i];
			float currentHeight = node->Cell->RelativeHeight;
			bool heightIncreased = (currentHeight > lastHeight);
			bool heightDecreased = (currentHeight < lastHeight);

			// Check if direction changed → always split
			if (node->NextDirection != lastDir)
			{
				path.push_back(*node->Cell);
				lastDir = node->NextDirection;
				lastHeight = currentHeight;
				trendInitialized = false;  // Reset trend for new direction
				continue;
			}

			// If direction is the same, check height trend
			if (!trendInitialized)
			{
				// First step: determine trend
				if (heightIncreased)
				{
					isIncreasing = true;
					isDecreasing = false;
					trendInitialized = true;
				}
				else if (heightDecreased)
				{
					isIncreasing = false;
					isDecreasing = true;
					trendInitialized = true;
				}
				// Else: neutral (same height), no trend yet
			}
			else
			{
				// Check if trend is violated
				if ((isIncreasing && !heightIncreased) || (isDecreasing && !heightDecreased))
				{
					// Trend broken → add to path and reset
					path.push_back(*node->Cell);
					trendInitialized = false;
					lastHeight = currentHeight;
					continue;
				}
			}

			// If we reach here, the trend is maintained → update lastHeight
			lastHeight = currentHeight;
		}

		return path;
	}

	bool PathFinder::IsEnd(PathNode* node) const
	{
		return mEnd == node; 
	}

	glm::ivec2 PathFinder::ConvertDirectionToCoordOffset(ExplorationDirection direction) const
	{
		switch (direction)
		{
			case ExplorationDirection::None:
				return { 0, 0 };
			case ExplorationDirection::North:
				return {  0,  1 };
			case ExplorationDirection::South:
				return {  0, -1 };
			case ExplorationDirection::East:
				return {  1,  0 };
			case ExplorationDirection::West:
				return { -1,  0 };
			case ExplorationDirection::NorthWest:
				return { -1,  1 };
			case ExplorationDirection::NorthEast:
				return {  1,  1 };
			case ExplorationDirection::SouthEast:
				return {  1, -1 };
			case ExplorationDirection::SouthWest:
				return { -1, -1 };
			default:
				assert(false);
				return {};
		}
	}

	PathNode& PathFinder::GetAssociatedNode(NavCell cell)
	{
		if (mNodePool.count(cell.Index))
		{
			return mNodePool.at(cell.Index);
		}

		PathNode node;
		node.Cell = &mGrid->GetCell(cell.NavCoords);

		return mNodePool[cell.Index] = node;
	}

	std::deque<PathNode*>::const_iterator PathFinder::FindNodeWithLeastCost() const
	{
		return std::min_element(
			mToExplore.cbegin(),
			mToExplore.cend(),
			[](const PathNode* left, const PathNode* right) -> bool
			{
				return left->CalculateScore() < right->CalculateScore();
			});
	}

	float PathFinder::CalculateHeuristic(const NavCell& cell) const
	{
		return DistanceCost().Evaluate(cell, *mEnd->Cell);
	}
}
