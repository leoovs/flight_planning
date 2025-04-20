#pragma once

#include <cinttypes>

#include <deque>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

namespace uavpf
{
	class NavGrid;
	class NavNode;

	class AStarAlgorithm
	{
	public:
		void Initialize(
			NavGrid* grid,
			const glm::ivec2& start,
			const glm::ivec2& end);

		bool IsExplorable() const;
		bool IsGoal() const;

		void ExploreNext();
		void ExploreNeighbour(const glm::ivec2& direction);

		std::vector<NavNode*> ConstructPath() const;

	private:
		NavGrid* mGrid = nullptr;
		std::deque<NavNode*> mToExplore;
		std::unordered_set<NavNode*> mExplored;
		glm::ivec2 mStart;
		glm::ivec2 mEnd;
		NavNode* mCurrentNode = nullptr;
		NavNode* mTargetNode = nullptr;
	};
}

