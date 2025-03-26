#include "uavpf/terrain/terrain_mesh_builder.h"

#include <cassert>

namespace uavpf
{
	TerrainMeshBuilder::TerrainMeshBuilder(HeightMap& heightMap)
		: mHeightMap(&heightMap)
		, mNumberOfTriangleStrips(heightMap.GetHeight() - 1)
		, mNumberOfVerticesPerTriangleStrip(heightMap.GetWidth() * 2)
	{
		GeneratePositionsAndIndices();
	}

	TerrainMesh TerrainMeshBuilder::Build()
	{
		return TerrainMesh(
			std::move(mPositions),
			std::move(mIndices),
			mNumberOfTriangleStrips,
			mNumberOfVerticesPerTriangleStrip);
	}

	void TerrainMeshBuilder::GeneratePositionsAndIndices()
	{
		if (nullptr == mHeightMap)
		{
			return;
		}

		mPositions.clear();
		mIndices.clear();

		int32_t width = mHeightMap->GetWidth();
		int32_t height = mHeightMap->GetHeight();

		mPositions.reserve(width * height);
		mIndices.reserve(width * height * 3);

		for (int32_t i = 0; i < height; i++)
		{
			for (int32_t j = 0; j < width; j++)
			{
				float elevation = (*mHeightMap)[i][j];
				glm::vec4 position(-height / 2.0f + i, elevation, -width / 2.0f + j, 1.0f);
				mPositions.push_back(position);

				if (i < height - 1)
				{
					mIndices.push_back(j + width * (i + 0));
					mIndices.push_back(j + width * (i + 1));
				}
			}
		}
	}
}

