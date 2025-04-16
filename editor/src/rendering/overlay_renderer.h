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

		void RenderLine3D(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color);
		void RenderCircle3D(const glm::vec3& position, float radius, const glm::vec3& color);

	private:
		enum ShaderIndex
		{
			LineVS,
			LinePS,
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

		GraphicsContext* mGraphics = nullptr;
		Camera mCamera;
		std::array<ShaderSource, ShaderIndexCount> mShaderSources = GetShaderSources();
		std::array<Shader*, ShaderIndexCount> mShaders{};
		VertexInput* mLineVertexInput = nullptr;
	};
}

