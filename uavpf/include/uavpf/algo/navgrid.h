#pragma once

#include <cinttypes>

#include <vector>

#include <glm/glm.hpp>

namespace uavpf
{
	struct NavGridSpecification
	{
		int32_t Width = 1000;
		int32_t Depth = 1000;
	};

	struct NavNode
	{
		static constexpr float cInfiniteScore = 1e5f;

		float TotalCost = cInfiniteScore;
		float HeuristicCost = 0.0f;
		NavNode* Parent = nullptr;

		float GetScore() const
		{
			return TotalCost + HeuristicCost;
		}
	};

	class HeightMap;

	class NavGrid
	{
	public:
		NavGrid(NavGridSpecification spec);

		const HeightMap* GetHeightMap() const;
		void SetHeightMap(const HeightMap* heightMap);

		const NavGridSpecification& GetSpecification() const;

		float GetElevation(int32_t navX, int32_t navZ) const;
		NavNode* GetNode(int32_t navX, int32_t navZ); 

		glm::ivec2 GetCoordinates(const NavNode* node) const;
		glm::ivec2 ConvertCoordinates(const glm::ivec2& navCoords) const;

	private:
		NavGridSpecification mSpec;	
		const HeightMap* mHeightMap = nullptr;
		std::vector<NavNode> mGraph;
	};
}

