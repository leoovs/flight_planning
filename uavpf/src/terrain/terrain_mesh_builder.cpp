#include "uavpf/terrain/terrain_mesh_builder.h"

#include <cassert>

namespace uavpf
{
	TerrainMeshBuilder::TerrainMeshBuilder()
	{
		GeneratePositionsAndIndices();
	}

	TerrainMeshBuilder& TerrainMeshBuilder::SetHeight(
		const HeightMap& heightMap)
	{
		mHeightMap = &heightMap;
		return *this;
	}

	TerrainMeshBuilder& TerrainMeshBuilder::SetTransformation(
		const glm::mat4& transformation)
	{
		mTransformation = transformation;
		return *this;
	}

	TerrainMesh TerrainMeshBuilder::Build()
	{
		GeneratePositionsAndIndices();

		mNumberOfTriangleStrips = mHeightMap->GetDepth() - 1;
		mNumberOfVerticesPerTriangleStrip = mHeightMap->GetWidth() * 2;

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
		int32_t height = mHeightMap->GetDepth();

		mPositions.reserve(width * height);
		mIndices.reserve(width * height * 3);

		for (int32_t i = 0; i < height; i++)
		{
			for (int32_t j = 0; j < width; j++)
			{
				float k = mHeightMap->GetElevation(j, i);
				glm::vec4 rasterSpacePosition(j, k, i, 1.0f);
				glm::vec4 modelSpacePosition(mTransformation * rasterSpacePosition);

				mPositions.push_back(modelSpacePosition);

				if (i < height - 1)
				{
					mIndices.push_back(j + width * (i + 0));
					mIndices.push_back(j + width * (i + 1));
				}
			}
		}
	}
}

