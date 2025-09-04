#include "uavpf/nav/path_finder.h"
#include "uavpf/nav/exploration_direction.h"
#include "uavpf/debug/logger_provider.h"

#include <glm/ext/scalar_constants.hpp>

#include <algorithm>

namespace uavpf
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
			+ ExplorationDirectionToCoordOffset(direction);
		if (!mGrid->IsInBounds(neighbourCoords))
		{
			return;
		}

		PathNode& neighbour = GetAssociatedNode(mGrid->GetCell(neighbourCoords));

		StepContext step;
		step.Source = *mCurrent->Cell;
		step.Destination = *neighbour.Cell;
		step.ParentDirection = mCurrent->CameFromDirection;
		step.CurrentDirection = direction;

		float tentativeScore = mCurrent->TotalCost + mCost->Evaluate(step);

		if (tentativeScore < neighbour.TotalCost)
		{
			neighbour.Parent = mCurrent;
			neighbour.TotalCost = tentativeScore;
			neighbour.HeuristicCost = CalculateHeuristic(*neighbour.Cell);
			neighbour.CameFromDirection = direction;

			auto it = std::find(mToExplore.begin(), mToExplore.end(), &neighbour);	
			if (mToExplore.end() == it)
			{
				mToExplore.push_back(&neighbour);
			}
		}
	}

	std::vector<NavCell> PathFinder::ReconstructPath() const
	{
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
		ExplorationDirection currentDir = current->CameFromDirection;
		float heightGain = 0.0f;

		while (nullptr != current)
		{
			PathNode* parent = current->Parent;
			if (nullptr == parent)
			{
				path.push_back(*current->Cell);
				break;
			}

			float nextHeight = parent->Cell->RelativeHeight;
			float currentHeightGain = nextHeight - currentHeight;
			ExplorationDirection nextDir = parent->CameFromDirection;
			
			constexpr float cHeightSensitivity = 1e-2f;
			
			float heightDiff = nextHeight - currentHeight;

			bool dirDiffers = nextDir != currentDir;
			bool heightGainLowered = currentHeightGain - heightGain < -cHeightSensitivity; 
			bool parentIsEnd = current->Cell == mEnd->Cell;

			bool cannotTruncate = dirDiffers || heightGainLowered || parentIsEnd;
			if (cannotTruncate)
			{
				path.push_back(*current->Cell);	
			}

			currentDir = nextDir;
			currentHeight = nextHeight;
			heightGain = currentHeightGain;

			current = parent;
		}

		return path;
	}

	std::vector<NavCell> PathFinder::ReconstructAndTruncatePath2() const
	{
		std::vector<NavCell> path = ReconstructPath();

		std::vector<NavCell> truncatedPath;
		truncatedPath.reserve(path.size());

		NavCell current = path.front();
		truncatedPath.push_back(current);

		float currentHeight = current.RelativeHeight;
		float prevHeightGain = 0.0f;
		int32_t prevHeightTrend = 0;
		
		for (ptrdiff_t iCell = 1; iCell < path.size(); iCell++)
		{
			const NavCell& next = path.at(iCell);

			constexpr float cHeightPrecision = 1e-2f;

			float heightDifference = currentHeight - next.RelativeHeight;

			float gainRelativeToCurrent = next.RelativeHeight - current.RelativeHeight;

			int32_t currentHeightTrend = 0;
			if (heightDifference < -cHeightPrecision)
			{
				currentHeightTrend = -1;
			}
			else if (heightDifference > cHeightPrecision)
			{
				currentHeightTrend = 1;
			}

			bool heightTrendMismatch = currentHeightTrend != prevHeightTrend;
			bool heightGainLess = heightTrendMismatch
				? glm::abs(gainRelativeToCurrent - prevHeightGain) < cHeightPrecision
				: false;
			bool dirMismatch = mNodePool.at(current.Index).CameFromDirection
				!= mNodePool.at(next.Index).CameFromDirection;
			bool isEnd = &path.back() == &next;

			bool cannotTruncate = heightTrendMismatch
				|| heightGainLess
				|| dirMismatch
				|| isEnd;

			if (cannotTruncate)
			{
				truncatedPath.push_back(next);
				current = next;
			}

			prevHeightTrend = currentHeightTrend;
			prevHeightGain = gainRelativeToCurrent;
			currentHeight = next.RelativeHeight;
		}

		return truncatedPath;
	}

	bool PathFinder::IsEnd(PathNode* node) const
	{
		return mEnd == node; 
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
		NavCell endCell = *mEnd->Cell;
		return glm::length(glm::vec2(cell.NavCoords - endCell.NavCoords));
	}
}
