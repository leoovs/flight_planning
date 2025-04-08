#include "rendering/terrain_render_mesh.h"

namespace editor
{
	TerrainRenderMesh::TerrainRenderMesh(
		GraphicsContext* graphics, 
		const uavpf::TerrainMesh& mesh)
		: mGraphics(graphics)
	{
		LoadTerrain(mesh);
	}

	TerrainRenderMesh::~TerrainRenderMesh()
	{
		UnloadTerrain();
	}

	VertexInput* TerrainRenderMesh::GetTerrainInput() const
	{
		return mTerrainInput;
	}

	int32_t TerrainRenderMesh::GetNumberOfTriangleStrips() const
	{
		return mNumberOfTriangleStrips;
	}

	int32_t TerrainRenderMesh::GetNumberOfVerticesPerStrip() const
	{
		return mNumberOfVerticesPerStrip;
	}

	void TerrainRenderMesh::LoadTerrain(const uavpf::TerrainMesh& mesh)
	{
		mNumberOfTriangleStrips = mesh.GetNumberOfTriangleStrips();
		mNumberOfVerticesPerStrip = mesh.GetNumberOfVerticesPerTriangleStrip();

		GraphicsBufferParams vertexBufferParams;
		vertexBufferParams.DebugName = "Terrain render mesh vertex buffer";
		vertexBufferParams.StructSize = sizeof(float[4]);
		vertexBufferParams.StructCount = mesh.GetPositionCount();
		vertexBufferParams.Target = GraphicsBufferTarget::Vertex;

		mVertexBuffer = mGraphics->CreateBuffer(std::move(vertexBufferParams));
		mVertexBuffer->SetData(mesh.GetPositionData(), mesh.GetPositionCount() * sizeof(float[4]));

		GraphicsBufferParams indexBufferParams;
		indexBufferParams.DebugName = "Terrain render mesh index buffer";
		indexBufferParams.StructSize = sizeof(uint32_t);
		indexBufferParams.StructCount = mesh.GetIndexCount();
		indexBufferParams.Target = GraphicsBufferTarget::Index;

		mIndexBuffer = mGraphics->CreateBuffer(std::move(indexBufferParams));
		mIndexBuffer->SetData(mesh.GetIndexData(), mesh.GetIndexCount() * sizeof(uint32_t));

		VertexInputParams inputParams;
		inputParams.DebugName = "Terrain render mesh vertex input";
		inputParams.VertexBuffers[0] = mVertexBuffer;
		inputParams.IndexBuffer = mIndexBuffer;
		inputParams.VertexAttributes = {
			{ "Position", GraphicsFormat::R32G32B32A32_FLOAT, 0, 0 }
		};

		mTerrainInput = mGraphics->CreateVertexInput(std::move(inputParams));
	}

	void TerrainRenderMesh::UnloadTerrain()
	{
		mGraphics->DestroyVertexInput(mTerrainInput);
		mTerrainInput = nullptr;

		mGraphics->DestroyBuffer(mIndexBuffer);
		mIndexBuffer = nullptr;

		mGraphics->DestroyBuffer(mVertexBuffer);
		mVertexBuffer = nullptr;
	}
}

