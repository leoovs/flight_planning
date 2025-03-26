#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace uavpf
{
	class TerrainMesh
	{
	public:
		TerrainMesh() = default;

		TerrainMesh(
			std::vector<glm::vec4> positions,
			std::vector<uint32_t> indices,
			int32_t numberOfTriangleStrips,
			int32_t numberOfVerticesPerTriangleStrip);

		size_t GetPositionCount() const;
		const glm::vec4* GetPositionData() const;

		size_t GetIndexCount() const;
		const uint32_t* GetIndexData() const;

		int32_t GetNumberOfTriangleStrips() const;
		int32_t GetNumberOfVerticesPerTriangleStrip() const;

	private:
		std::vector<glm::vec4> mPositions;
		std::vector<uint32_t> mIndices;
		int32_t mNumberOfTriangleStrips = 0;
		int32_t mNumberOfVerticesPerTriangleStrip = 0;
	};
}

