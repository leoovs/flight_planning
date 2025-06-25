#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/exploration_direction.h"

#include <algorithm>

namespace uavpf::experimental
{
	PathFinder::PathFinder(
		const NavGrid& grid,
		NavCell start,
		NavCell end,
		const StepCost& cost)
		: mGrid(&grid)
		, mEnd(nullptr)
		, mCost(&cost)
	{
		PathNode& startNode = GetAssociatedNode(start);
		startNode.TotalCost = 0.0f;
		startNode.HeuristicCost = CalculateHeuristic(start);

		mEnd = &GetAssociatedNode(end);	

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
		mCurrent->NextDirection = direction;
		glm::ivec2 neighbourCoords = mCurrent->Cell->NavCoords
			+ ConvertDirectionToCoordOffset(direction);

		PathNode& neighbour = GetAssociatedNode(mGrid->GetCell(neighbourCoords));

		float tentativeScore = mCurrent->TotalCost + mCost->Evaluate(
			*mCurrent->Cell,
			*neighbour.Cell);

		if (tentativeScore < neighbour.TotalCost)
		{
			neighbour.Parent = mCurrent;
			neighbour.TotalCost = tentativeScore;
			neighbour.HeuristicCost = tentativeScore + CalculateHeuristic(*neighbour.Cell);
			auto it = std::find(mToExplore.begin(), mToExplore.end(), &neighbour);	
			if (mToExplore.end() != it)
			{
				mToExplore.push_back(&neighbour);
			}
		}
	}

	std::vector<NavCell> PathFinder::ReconstructPath() const
	{
		std::vector<NavCell> path;
		PathNode* current = mCurrent;
		ExplorationDirection dir = current->NextDirection;
		
		while (current != nullptr)
		{
			path.push_back(*current->Cell);

			current = current->Parent;
			dir = current->NextDirection;

			while (nullptr != current && current->NextDirection == dir)
			{
				current = current->Parent;
				dir = current->NextDirection;
			}
		}

		std::reverse(path.begin(), path.end());

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
		}
	}

	PathNode& PathFinder::GetAssociatedNode(const NavCell& cell)
	{
		if (mNodePool.size() <= cell.Index)
		{
			mNodePool.resize(cell.Index + 1);
		}
		return mNodePool.at(cell.Index);
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
		return glm::distance(glm::vec2(cell.NavCoords), glm::vec2(mEnd->Cell->NavCoords));
	}
}
