#include "rendering/overlay_renderer.h"
#include "graphics/shader_compiler.h"
#include "uavpf/debug/logger_provider.h"

#include <glm/gtx/string_cast.hpp>

namespace editor
{
	OverlayRenderer::OverlayRenderer(GraphicsContext* graphics)
		: mGraphics(graphics)
	{
		CreateShaders();
		CreateLineVertexInput();
	}

	OverlayRenderer::~OverlayRenderer()
	{
		DestroyLineVertexInput();
		DestroyShaders();
	}

	void OverlayRenderer::SetCamera(const Camera& camera)
	{
		mCamera = camera;
	}

	void OverlayRenderer::RenderLine3D(const glm::vec3& a, const glm::vec3& b)
	{
		glm::mat4 model(1.0f);
		glm::mat4 modelA = glm::translate(model, a);
		glm::mat4 modelB = glm::translate(model, b);

		glm::mat4 view = mCamera.GetLookAtMatrix();
		glm::mat4 proj = mCamera.GetProjectionMatrix();

		glm::vec4 clipSpaceA = proj * view * modelA * glm::vec4(glm::vec3(0.0f), 1.0f);
		glm::vec4 clipSpaceB = proj * view * modelB * glm::vec4(glm::vec3(0.0f), 1.0f);

		glm::vec4 ndcA = clipSpaceA / clipSpaceA.w;
		glm::vec4 ndcB = clipSpaceB / clipSpaceB.w;

		UAVPF_LOG(
			Application,
			Trace,
			"Pos: %s",
			glm::to_string(ndcA).data());

		const Viewport& vp = mGraphics->GetViewport();
		float width = float(vp.Width - vp.TopLeftX);
		float height = float(vp.Height - vp.TopLeftY);

		mShaders.at(LinePS)->SetUniform("uPointA", ndcA);
		mShaders.at(LinePS)->SetUniform("uPointB", ndcB);
		mShaders.at(LinePS)->SetUniform("uViewport", width, height);

		mGraphics->SetVertexInput(mLineVertexInput);
		mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleList);
		mGraphics->SetShader(ShaderKind::Vertex, mShaders.at(LineVS));
		mGraphics->SetShader(ShaderKind::Pixel, mShaders.at(LinePS));

		mGraphics->Draw(0, 6);
	}

	void OverlayRenderer::RenderCircle3D(
		const glm::vec3& position,
		float radius)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(radius));

		glm::mat4 view = mCamera.GetLookAtMatrix();
		glm::mat4 proj = mCamera.GetProjectionMatrix();

		glm::vec4 clipSpace = proj * view * model * glm::vec4(glm::vec3(0.0f), 1.0f);

		mShaders.at(CircleVS)->SetUniform("uModel", model);
		mShaders.at(CircleVS)->SetUniform("uView", view);
		mShaders.at(CircleVS)->SetUniform("uProj", proj);

		mGraphics->SetVertexInput(mLineVertexInput);
		mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleList);
		mGraphics->SetShader(ShaderKind::Vertex, mShaders.at(CircleVS));
		mGraphics->SetShader(ShaderKind::Pixel, mShaders.at(CirclePS));

		mGraphics->Draw(0, 6);
	}

	auto OverlayRenderer::GetShaderSources() -> std::array<ShaderSource, ShaderIndexCount>
	{
		std::array<ShaderSource, ShaderIndexCount> sources;

		sources.at(LineVS).Code = R"(
			#version 460 core

			out gl_PerVertex
			{
				vec4 gl_Position;
			};

			vec4[] quadPositions = vec4[4](
				vec4(-1.0f, -1.0f, 0.0f, 1.0f),	
				vec4(-1.0f,  1.0f, 0.0f, 1.0f),	
				vec4( 1.0f,  1.0f, 0.0f, 1.0f),	
				vec4( 1.0f, -1.0f, 0.0f, 1.0f)
			);
			uint[] quadIndices = uint[6](0, 1, 2, 0, 2, 3);

			void main()
			{
				uint index = quadIndices[gl_VertexID];
				gl_Position = quadPositions[index];
			}
		)";
		sources.at(LineVS).Kind = ShaderKind::Vertex;

		sources.at(LinePS).Code = R"(
			#version 460 core
			
			uniform vec4 uPointA;
			uniform vec4 uPointB;
			uniform vec2 uViewport;

			out vec4 oColor;

			float SolveSlope(vec4 a, vec4 b)
			{
				return (a.y - b.y) / (a.x - b.x);
			}

			float SolveOffset(vec4 a, vec4 b, float slope)
			{
				return a.y - slope * a.x;
			}

			void main()
			{
				vec4 left = uPointA;
				vec4 right = uPointB;

				if (left.z < -1.0f && right.z < -1.0f
					|| left.z > 1.0f && right.z > 1.0f)
				{
					discard;
				}

				vec2 ndc;
				ndc.x = (gl_FragCoord.x / uViewport.x - 0.5) * 2.0;
				ndc.y = (gl_FragCoord.y / uViewport.y - 0.5) * 2.0;

				if (right.x < left.x)
				{
					vec4 tmp = left;
					left = right;
					right = tmp;
				}

				if (ndc.x < left.x || ndc.x > right.x)
				{
					discard;
				}

				float eps = 1e-2f;

				// float k = (left.y - right.y) / (left.x - right.x);
				// float b = left.y - k * left.x;
				float xoy_k = SolveSlope(left, right);
				float xoy_b = SolveOffset(left, right, xoy_k);

				vec4 test;
				test.y = xoy_k * ndc.x + xoy_b;
				test.x = (test.y - xoy_b) / xoy_k;

				if (abs(test.x - ndc.x) > eps || abs(test.y - ndc.y) > eps)
				{
					discard;
				}

				oColor = vec4(1.0f);
			}
		)";
		sources.at(LinePS).Kind = ShaderKind::Pixel;

		sources.at(CircleVS).Code = R"(
			#version 460 core

			uniform mat4 uModel;
			uniform mat4 uView;
			uniform mat4 uProj;

			out vec2 uv;

			out gl_PerVertex
			{
				vec4 gl_Position;
			};

			vec2[] quadUVs = vec2[4](
				vec2(0.0f, 0.0f),
				vec2(0.0f, 1.0f),
				vec2(1.0f, 1.0f),
				vec2(1.0f, 0.0f)
			);

			vec4[] quadPositions = vec4[4](
				vec4(-1.0f, -1.0f, 0.0f, 1.0f),	
				vec4(-1.0f,  1.0f, 0.0f, 1.0f),	
				vec4( 1.0f,  1.0f, 0.0f, 1.0f),	
				vec4( 1.0f, -1.0f, 0.0f, 1.0f)
			);
			uint[] quadIndices = uint[6](0, 1, 2, 2, 3, 0);

			float GetScalingFactor(mat4 m)
			{
				float xx2 = pow(m[0][0], 2);
				float yx2 = pow(m[1][0], 2);
				float zx2 = pow(m[2][0], 2);

				return sqrt(xx2 + yx2 + zx2);
			}

			mat4 EraseRotation(mat4 m)
			{
				float t1 = m[3][0];
				float t2 = m[3][1];
				float t3 = m[3][2];

				float d = GetScalingFactor(m);

				mat4 result;

				result[0] = vec4(d, 0.0f, 0.0f, 0.0f);
				result[1] = vec4(0.0f, d, 0.0f, 0.0f);
				result[2] = vec4(0.0f, 0.0f, d, 0.0f);
				result[3] = vec4(t1, t2, t3, 1.0f);

				return result;
			}

			void main()
			{
				uint index = quadIndices[gl_VertexID];
				vec4 position = quadPositions[index];
				uv = quadUVs[index];

				mat4 MVP = uProj * EraseRotation(uView * uModel);

				gl_Position = MVP * position;
			}
		)";
		sources.at(CircleVS).Kind = ShaderKind::Vertex;

		sources.at(CirclePS).Code = R"(
			#version 460 core

			in vec2 uv;

			out vec4 oColor;

			void main()
			{
				float distance = length(uv - vec2(0.5f, 0.5f));
				if (distance > 0.5f)
				{
					discard;
				}

				oColor = vec4(uv, 0.0f, 1.0f);
			}
		)";
		sources.at(CirclePS).Kind = ShaderKind::Pixel;
		
		return sources;
	}

	void OverlayRenderer::CreateShaders()
	{
		ShaderCompiler* compiler = mGraphics->GetShaderCompiler();
		for (size_t iShader = 0; iShader < ShaderIndexCount; iShader++)
		{
			const ShaderSource& source = mShaderSources.at(iShader);

			ShaderCompilation* compilation = compiler->Compile(source.Kind, source.Code);
			if (compilation->GetStatus() != ShaderCompilationStatus::Success)
			{
				UAVPF_LOG(
					Application,
					Error,
					"Internal error: overlay renderer failed to compile its shaders:\n%s",
					compilation->GetDiagnostics().data());
				compiler->DestroyCompilation(compilation);
				continue;
			}

			mShaders.at(iShader) = mGraphics->CreateShader(compilation);
			compiler->DestroyCompilation(compilation);
		}
	}

	void OverlayRenderer::DestroyShaders()
	{
		for (Shader*& shader : mShaders)
		{
			mGraphics->DestroyShader(shader);
			shader = nullptr;
		}
	}

	void OverlayRenderer::CreateLineVertexInput()
	{
		VertexInputParams params;
		params.DebugName = "Overlay renderer vertex input";

		mLineVertexInput = mGraphics->CreateVertexInput(std::move(params));
	}

	void OverlayRenderer::DestroyLineVertexInput()
	{
		mGraphics->DestroyVertexInput(mLineVertexInput);
	}
}

