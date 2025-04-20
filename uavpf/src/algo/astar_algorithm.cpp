#include "uavpf/algo/astar_algorithm.h"

#include "uavpf/algo/navgrid.h"

#include <algorithm>

namespace uavpf
{
	void AStarAlgorithm::Initialize(
		NavGrid* grid,
		const glm::ivec2& start,
		const glm::ivec2& end)
	{
		mGrid = grid;
		mToExplore.clear();
		mStart = start;
		mEnd = end;
		mTargetNode = mGrid->GetNode(end.x, end.y);

		NavNode* startNode = mGrid->GetNode(start.x, start.y);
		startNode->TotalCost = 0.0f;
		startNode->HeuristicCost = glm::length(glm::vec2(start - end));
		mToExplore.push_back(startNode);
	}

	bool AStarAlgorithm::IsExplorable() const
	{
		return !mToExplore.empty();
	}

	bool AStarAlgorithm::IsGoal() const
	{
		return mCurrentNode == mTargetNode;
	}

	void AStarAlgorithm::ExploreNext()
	{
		auto minIt = std::min_element(
			mToExplore.begin(),
			mToExplore.end(),
			[](const NavNode* left, const NavNode* right) -> bool
			{
				return left->GetScore() < right->GetScore();
			}
		);
		mCurrentNode = *minIt;

		if (IsGoal())
		{
			return;	
		}

		mToExplore.erase(minIt);
		mExplored.insert(mCurrentNode);
	}

	void AStarAlgorithm::ExploreNeighbour(const glm::ivec2& direction)
	{
		glm::ivec2 currentCoords = mGrid->GetCoordinates(mCurrentNode);					
		glm::ivec2 neighbourCoords = currentCoords + direction;

		NavNode* neighbour = mGrid->GetNode(neighbourCoords.x, neighbourCoords.y);
		if (neighbour == nullptr)
		{
			return;
		}

		float totalScoreFromStart = mCurrentNode->TotalCost + glm::length(glm::vec2(direction));
		if (totalScoreFromStart < neighbour->TotalCost)
		{
			neighbour->Parent = mCurrentNode;
			neighbour->TotalCost = totalScoreFromStart;
			neighbour->HeuristicCost = glm::length(glm::vec2(neighbourCoords - mEnd));

			auto it = std::find(mToExplore.begin(), mToExplore.end(), neighbour);
			if (it == mToExplore.end())
			{
				mToExplore.push_back(neighbour);
			}
		}
	}

	std::vector<NavNode*> AStarAlgorithm::ConstructPath() const
	{
		std::vector<NavNode*> path;
		NavNode* current = mCurrentNode;
		while (current != nullptr)
		{
			path.push_back(current);
			current = current->Parent;
		}

		return path;
	}
}
