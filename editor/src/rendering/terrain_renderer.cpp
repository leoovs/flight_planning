#include "rendering/terrain_renderer.h"

#include "graphics/depth_stencil_state.h"
#include "graphics/framebuffer.h"
#include "rendering/terrain_render_mesh.h"
#include "uavpf/debug/logger_provider.h"

namespace editor
{
	TerrainRenderer::TerrainRenderer(GraphicsContext* graphics)
		: mGraphics(graphics)
	{
		CreateFramebuffer();
		CreateShaders();
	}

	TerrainRenderer::TerrainRenderer(TerrainRenderer&& other) noexcept
		: mGraphics(other.mGraphics)
		, mFramebuffer(std::exchange(other.mFramebuffer, nullptr))
		, mShaders(std::exchange(other.mShaders, {}))
	{
	}

	TerrainRenderer& TerrainRenderer::operator=(TerrainRenderer&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		mGraphics = other.mGraphics;
		mFramebuffer = std::exchange(other.mFramebuffer, nullptr);
		mShaders = std::exchange(other.mShaders, {});

		return *this;
	}

	TerrainRenderer::~TerrainRenderer()
	{
		DestroyShaders();
		DestroyFramebuffer();
	}

	auto TerrainRenderer::GetShaderSources() -> std::array<ShaderSource, ShaderIndexCount>
	{
		std::array<ShaderSource, ShaderIndexCount> sources;

		sources.at(TerrainVS).Code = R"(
			#version 460 core

			uniform mat4 uModel;
			uniform mat4 uViewProj;
			uniform vec3 uEyePosition;

			layout (location = 0) in vec3 aPosition;

			out vec4 modelSpacePosition;
			out vec4 worldSpacePosition;
			out vec3 eyeRelativePosition;

			out gl_PerVertex
			{
				vec4 gl_Position;
			};

			void main()
			{
				modelSpacePosition = vec4(aPosition, 1.0);
				worldSpacePosition = uModel * modelSpacePosition;

				eyeRelativePosition = worldSpacePosition.xyz - uEyePosition;

				gl_Position = uViewProj * worldSpacePosition;
			}
		)";
		sources.at(TerrainVS).Kind = ShaderKind::Vertex;

		sources.at(TerrainPS).Code = R"(
			#version 460 core

			uniform vec3 uEyePosition;

			in vec4 modelSpacePosition;
			in vec4 worldSpacePosition;
			in vec3 eyeRelativePosition;

			out vec4 oColor;

			vec3 getNormal()
			{
				vec3 dPosdX = dFdx(eyeRelativePosition);
				vec3 dPosdY = dFdy(eyeRelativePosition);
				
				vec3 faceNormal = normalize(cross(dPosdX, dPosdY));
				return faceNormal;
			}

			void main()
			{
				vec3 normal = getNormal();
				vec3 lightDir =  normalize(uEyePosition - vec3(worldSpacePosition));

				float height = modelSpacePosition.y;
				float light = max(0.4, dot(normalize(lightDir), normalize(normal)));

				vec3 color = vec3(0.3f, height, 1 - height) * light * (height+0.3f) * 1.5f;

				oColor = vec4(color, 1.0);
			}
		)";
		sources.at(TerrainPS).Kind = ShaderKind::Pixel;

		return sources;
	}

	void TerrainRenderer::CreateFramebuffer()
	{
		FramebufferParams params;
		params.DebugName = "Terrain renderer framebuffer";

		mFramebuffer = mGraphics->CreateFramebuffer(std::move(params));
	}

	TerrainRenderMesh* TerrainRenderer::CreateTerrainRenderMesh(const uavpf::TerrainMesh& mesh)
	{
		return new TerrainRenderMesh(mGraphics, mesh);
	}

	void TerrainRenderer::DestroyTerrainRenderMesh(TerrainRenderMesh* mesh)
	{
		delete mesh;
	}

	void TerrainRenderer::SetTargetTextures(
		Texture2D* colorBuffer,
		Texture2D* depthStencilBuffer)
	{
		SubresourceRegion region;
		region.MipLevelIndex = 0;

		mFramebuffer->AttachTexture2D(FramebufferAttachment::Color, colorBuffer, region);
		mFramebuffer->AttachTexture2D(FramebufferAttachment::DepthStencil, depthStencilBuffer, region);
	}

	void TerrainRenderer::SetCamera(const Camera& camera)
	{
		mCamera = camera;

		mShaders
			.at(TerrainVS)
			->SetUniform(
			"uViewProj",
			camera.CalculateViewProjectionMatrix());

		mShaders
			.at(TerrainVS)
			->SetUniform(
			"uEyePosition",
			camera.GetPosition());

		mShaders
			.at(TerrainPS)
			->SetUniform(
			"uEyePosition",
			camera.GetPosition());
	}

	void TerrainRenderer::Clear()
	{
		mGraphics->ClearColor(mFramebuffer, 0.25f, 0.25f, 0.25f, 1.0f);
		mGraphics->ClearDepthStencil(mFramebuffer, 1.0f, 0);
	}

	void TerrainRenderer::Render(TerrainRenderMesh* mesh, const glm::mat4& modelMatrix)
	{
		DepthStencilState dsState;
		dsState.DepthTestEnabled = true;
		mGraphics->SetDepthStencilState(dsState);

		mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleStrip);
		mGraphics->SetVertexInput(mesh->GetTerrainInput());
		mGraphics->SetFramebuffer(mFramebuffer);

		mShaders.at(TerrainVS)->SetUniform("uModel", modelMatrix);

		mGraphics->SetShader(ShaderKind::Vertex, mShaders.at(TerrainVS));
		mGraphics->SetShader(ShaderKind::Pixel, mShaders.at(TerrainPS));

		for (int32_t strip = 0; strip < mesh->GetNumberOfTriangleStrips(); strip++)
		{
			mGraphics->DrawIndexed(
				mesh->GetNumberOfVerticesPerStrip() * strip,
				mesh->GetNumberOfVerticesPerStrip());
		}
	}

	void TerrainRenderer::DestroyFramebuffer()
	{
		if (nullptr != mFramebuffer)
		{
			mGraphics->DestroyFramebuffer(mFramebuffer);
			mFramebuffer = nullptr;
		}
	}

	void TerrainRenderer::CreateShaders()
	{
		ShaderCompiler* compiler = mGraphics->GetShaderCompiler();

		for (size_t iShader = 0; iShader < mShaderSources.size(); iShader++)
		{
			const ShaderSource& source = mShaderSources.at(iShader);

			ShaderCompilation* compilation = compiler->Compile(source.Kind, source.Code);
			if (compilation->GetStatus() != ShaderCompilationStatus::Success)
			{
				UAVPF_LOG(
					Application,
					Error,
					"Internal terrain renderer error: could not compile shaders:\n%s",
					compilation->GetDiagnostics().data());

				compiler->DestroyCompilation(compilation);
				compilation = nullptr;
				continue;
			}

			mShaders.at(iShader) = mGraphics->CreateShader(compilation);

			compiler->DestroyCompilation(compilation);
			compilation = nullptr;
		}
	}

	void TerrainRenderer::DestroyShaders()
	{
		for (Shader*& shader : mShaders)
		{
			if (nullptr != shader)
			{
				mGraphics->DestroyShader(shader);
				shader = nullptr;
			}
		}
	}
}

