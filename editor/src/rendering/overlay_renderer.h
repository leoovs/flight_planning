#pragma once

#include <glm/glm.hpp>

#include "graphics/graphics_context.h"
#include "rendering/camera.h"

namespace editor
{
	class OverlayRenderer
	{
	public:
		OverlayRenderer(GraphicsContext* graphics);
		~OverlayRenderer();

		void SetCamera(const Camera& camera);
		void SetTargetTextures(Texture2D* colorBuffer, Texture2D* depthStencilBuffer);
		
		void RenderLine3D(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color);
		void RenderCircle3D(const glm::vec3& position, float radius, const glm::vec3& color);

		void IgnoreDepth(bool ignore);

	private:
		enum ShaderIndex
		{
			LineVS,
			LinePS,
			LineVS2,
			LinePS2,
			CircleVS,
			CirclePS,
			ShaderIndexCount
		};

		struct ShaderSource
		{
			const char* Code = nullptr;
			ShaderKind Kind = ShaderKind::Unknown;
		};

		static std::array<ShaderSource, ShaderIndexCount> GetShaderSources();

		void CreateShaders();
		void DestroyShaders();

		void CreateLineVertexInput();
		void DestroyLineVertexInput();

		void CreateFramebuffer();
		void DestroyFramebuffer();

		GraphicsContext* mGraphics = nullptr;
		Camera mCamera;
		std::array<ShaderSource, ShaderIndexCount> mShaderSources = GetShaderSources();
		std::array<Shader*, ShaderIndexCount> mShaders{};
		GraphicsBuffer* mLinesBuffer = nullptr;
		VertexInput* mLineVertexInput = nullptr;
		Framebuffer* mFramebuffer = nullptr;
		DepthStencilState mDepthStencilState;
	};
}

