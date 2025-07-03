#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/exploration_direction.h"

#include <algorithm>
#include <glm/ext/scalar_constants.hpp>

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

		PathNode& neighbour = GetAssociatedNode(mGrid->GetCell(neighbourCoords));

		float tentativeScore = mCurrent->TotalCost + mCost->Evaluate(
			*mCurrent->Cell,
			*neighbour.Cell);

		if (tentativeScore < neighbour.TotalCost)
		{
			neighbour.Parent = mCurrent;
			neighbour.TotalCost = tentativeScore;
			neighbour.HeuristicCost = CalculateHeuristic(*neighbour.Cell);
			neighbour.NextDirection = direction;

			auto it = std::find(mToExplore.begin(), mToExplore.end(), &neighbour);	
			if (mToExplore.end() == it)
			{
				mToExplore.push_back(&neighbour);
			}
		}
	}

	std::vector<NavCell> PathFinder::ReconstructPath() const
	{
		return ReconstructAndTruncatePath();

		std::vector<NavCell> path;
		PathNode* current = mCurrent;

		while (nullptr != current)
		{
			path.push_back(*current->Cell);
			current = current->Parent;
		}

		return path;
	}

	std::vector<NavCell> PathFinder::ReconstructAndTruncatePath() const
	{
		std::vector<NavCell> path;

		PathNode* current = mCurrent;
		if (nullptr == current)
		{
			return path;
		}

		float currentHeight = current->Cell->RelativeHeight;
		ExplorationDirection currentDir = current->NextDirection;
		int32_t heightTrend = -1;

		while (nullptr != current)
		{
			PathNode* parent = current->Parent;
			if (nullptr == parent)
			{
				path.push_back(*current->Cell);
				break;
			}

			float nextHeight = parent->Cell->RelativeHeight;
			ExplorationDirection nextDir = parent->NextDirection;
			int32_t nextHeightTrend = 0;
			
			float heightSensitivity = glm::epsilon<float>();
			heightSensitivity = 1e-1f;
			float heightDiff = nextHeight - currentHeight;

			if (heightDiff > heightSensitivity)
			{
				nextHeightTrend = 1;
			}
			else if (heightDiff < -heightSensitivity)
			{
				nextHeightTrend = 2;
			}

			bool dirDiffers = nextDir != currentDir;
			bool heightTrendDiffers = heightTrend != nextHeightTrend;

			bool anythingDiffers = dirDiffers || heightTrendDiffers;

			if (anythingDiffers)
			{
				path.push_back(*current->Cell);	
			}

			currentDir = nextDir;
			heightTrend = nextHeightTrend;
			currentHeight = nextHeight;

			current = parent;
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
