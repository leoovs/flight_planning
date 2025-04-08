#pragma once

#include <uavpf/terrain/terrain_mesh.h>
#include <glm/glm.hpp>

#include "graphics/graphics_context.h"

namespace editor
{
	class TerrainRenderMesh
	{
	public:
		TerrainRenderMesh(GraphicsContext* graphics, const uavpf::TerrainMesh& mesh);

		TerrainRenderMesh(const TerrainRenderMesh& other) = delete;
		TerrainRenderMesh& operator=(const TerrainRenderMesh& other) = delete;

		~TerrainRenderMesh();

		VertexInput* GetTerrainInput() const;
		int32_t GetNumberOfTriangleStrips() const;
		int32_t GetNumberOfVerticesPerStrip() const;

	private:
		void LoadTerrain(const uavpf::TerrainMesh& mesh);
		void UnloadTerrain();

		GraphicsContext* mGraphics = nullptr;	
		VertexInput* mTerrainInput = nullptr;
		GraphicsBuffer* mVertexBuffer = nullptr;
		GraphicsBuffer* mIndexBuffer = nullptr;
		int32_t mNumberOfTriangleStrips = 0;
		int32_t mNumberOfVerticesPerStrip = 0;
	};
}

