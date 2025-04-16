#pragma once

#include <uavpf/uavpf.h>

#include "graphics/graphics_context.h"
#include "rendering/camera.h"

namespace editor
{
	class TerrainRenderMesh;

	class TerrainRenderer
	{
	public:
		TerrainRenderer(GraphicsContext* graphics);

		TerrainRenderer(TerrainRenderer&& other) noexcept;
		TerrainRenderer& operator=(TerrainRenderer&& other) noexcept;

		~TerrainRenderer();

		// TODO: add clear method

		TerrainRenderMesh* CreateTerrainRenderMesh(const uavpf::TerrainMesh& mesh);
		void DestroyTerrainRenderMesh(TerrainRenderMesh* mesh);

		void SetTargetTextures(Texture2D* colorBuffer, Texture2D* depthStencilBuffer);
		void SetCamera(const Camera& camera);

		void Render(TerrainRenderMesh* mesh, const glm::mat4& modelMatrix);

	private:
		struct ShaderSource
		{
			const char* Code = nullptr;
			ShaderKind Kind = ShaderKind::Unknown;
		};

		enum ShaderIndex
		{
			TerrainVS,
			TerrainPS,

			ShaderIndexCount
		};

		static std::array<ShaderSource, ShaderIndexCount> GetShaderSources();

		void CreateFramebuffer();
		void DestroyFramebuffer();

		void CreateShaders();
		void DestroyShaders();

		GraphicsContext* mGraphics = nullptr;
		Framebuffer* mFramebuffer = nullptr;
		std::array<Shader*, ShaderIndexCount> mShaders{};
		std::array<ShaderSource, ShaderIndexCount> mShaderSources = GetShaderSources();

		Camera mCamera;
	};
}
