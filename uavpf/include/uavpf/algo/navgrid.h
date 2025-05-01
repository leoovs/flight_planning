#pragma once

#include <cinttypes>

#include <vector>
#include <limits>

#include <glm/glm.hpp>

namespace uavpf
{
	struct NavGridSpecification
	{
		int32_t Width = 0;
		int32_t Depth = 0;
	};

	struct NavNode
	{
		static constexpr float cInfiniteScore = std::numeric_limits<float>::infinity();

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
		NavGrid() = default;
		NavGrid(NavGridSpecification spec);

		const HeightMap* GetHeightMap() const;
		void SetHeightMap(const HeightMap* heightMap);

		const NavGridSpecification& GetSpecification() const;

		float GetElevation(int32_t navX, int32_t navZ) const;
		float GetElevation(const glm::ivec2& navCoords) const;

		NavNode* GetNode(int32_t navX, int32_t navZ); 
		NavNode* GetNode(const glm::ivec2& navCoords); 

		glm::ivec2 GetCoordinates(const NavNode* node) const;
		glm::ivec2 ConvertCoordinates(const glm::ivec2& navCoords) const;

	private:
		NavGridSpecification mSpec;	
		const HeightMap* mHeightMap = nullptr;
		std::vector<NavNode> mGraph;
	};
}

