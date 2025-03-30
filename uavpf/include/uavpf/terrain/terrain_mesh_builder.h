#pragma once

#include <glm/glm.hpp>

#include "uavpf/terrain/terrain_mesh.h"
#include "uavpf/terrain/height_map.h"

namespace uavpf
{
	class TerrainMeshBuilder
	{
	public:
		TerrainMeshBuilder();

		TerrainMeshBuilder& SetHeight(const HeightMap& heightMap);
		TerrainMeshBuilder& SetTransformation(const glm::mat4& transformation);
		TerrainMesh Build();
	
	private:
		void GeneratePositionsAndIndices();

		const HeightMap* mHeightMap = nullptr;
		glm::mat4 mTransformation = glm::mat4(1.0f);
		std::vector<glm::vec4> mPositions;
		std::vector<uint32_t> mIndices;
		int32_t mNumberOfTriangleStrips = 0;
		int32_t mNumberOfVerticesPerTriangleStrip = 0;
	};
}

