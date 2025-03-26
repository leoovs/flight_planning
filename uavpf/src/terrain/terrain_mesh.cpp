#include "uavpf/terrain/terrain_mesh.h"

namespace uavpf
{
	TerrainMesh::TerrainMesh(
		std::vector<glm::vec4> positions,
		std::vector<uint32_t> indices,
		int32_t numberOfTriangleStrips,
		int32_t numberOfVerticesPerTriangleStrip)
		: mPositions(std::move(positions))
		, mIndices(std::move(indices))
		, mNumberOfTriangleStrips(numberOfTriangleStrips)
		, mNumberOfVerticesPerTriangleStrip(numberOfVerticesPerTriangleStrip)
	{
	}

	size_t TerrainMesh::GetPositionCount() const
	{
		return mPositions.size();
	}

	const glm::vec4* TerrainMesh::GetPositionData() const
	{
		return mPositions.data();
	}

	size_t TerrainMesh::GetIndexCount() const
	{
		return mIndices.size();
	}

	const uint32_t* TerrainMesh::GetIndexData() const
	{
		return mIndices.data();
	}

	int32_t TerrainMesh::GetNumberOfTriangleStrips() const
	{
		return mNumberOfTriangleStrips;
	}

	int32_t TerrainMesh::GetNumberOfVerticesPerTriangleStrip() const
	{
		return mNumberOfVerticesPerTriangleStrip;
	}
}

