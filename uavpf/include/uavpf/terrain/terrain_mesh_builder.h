#pragma once

#include "uavpf/terrain/terrain_mesh.h"
#include "uavpf/terrain/height_map.h"

namespace uavpf
{
	class TerrainMeshBuilder
	{
	public:
		TerrainMeshBuilder() = default;
		TerrainMeshBuilder(HeightMap& heightMap);

		TerrainMesh Build();
	
	private:
		void GeneratePositionsAndIndices();

		HeightMap* mHeightMap = nullptr;
		std::vector<glm::vec4> mPositions;
		std::vector<uint32_t> mIndices;
		int32_t mNumberOfTriangleStrips = 0;
		int32_t mNumberOfVerticesPerTriangleStrip = 0;
	};
}

