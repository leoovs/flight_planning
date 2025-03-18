#include "graphics_ogl/ogl_graphics_context.h"

#include <cassert>

#include <uavpf/uavpf.h>
#include <GL/glew.h>

#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_graphics_debug_watch.h"
#include "graphics_ogl/ogl_shader_compilation.h"

namespace editor
{
	OglGraphicsContext::OglGraphicsContext(
		GraphicsContextParams params,
		std::unique_ptr<OglProvider> provider)
		: mParams(std::move(params))
		, mProvider(std::move(provider))
	{
		assert(nullptr != mProvider);

		BindGlobalShaderPipeline();
		SetNativeViewport(mViewport);
	}

	const GraphicsContextParams& OglGraphicsContext::GetParams() const
	{
		return mParams;
	}

	void OglGraphicsContext::Present()
	{
		mProvider->SwapBuffers();
	}

	void OglGraphicsContext::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	OglGraphicsDebugWatch* OglGraphicsContext::CreateDebugWatch()
	{
		if (nullptr != mRegisteredDebugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to create multiple graphics debug watches");
			return mRegisteredDebugWatch;
		}

		return mRegisteredDebugWatch = new OglGraphicsDebugWatch();
	}

	void OglGraphicsContext::DestroyDebugWatch(GraphicsDebugWatch* debugWatch)
	{
		if (nullptr == debugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy graphics debug watch which is nullptr");
		}

		if (debugWatch != mRegisteredDebugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy graphics debug watch context which is"
				" different from registered one");
		}

		delete debugWatch;
		mRegisteredDebugWatch = nullptr;
	}

	OglGraphicsBuffer* OglGraphicsContext::CreateBuffer(
		GraphicsBufferParams params)
	{
		return new OglGraphicsBuffer(std::move(params));
	}

	void OglGraphicsContext::DestroyBuffer(
		GraphicsBuffer* graphicsBuffer)
	{
		delete graphicsBuffer;
	}

	OglVertexInput* OglGraphicsContext::CreateVertexInput(VertexInputParams params)
	{
		return new OglVertexInput(std::move(params));
	}

	void OglGraphicsContext::DestroyVertexInput(VertexInput* vertexInput)
	{
		if (nullptr == vertexInput)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy nullptr vertex input");
		}
		delete vertexInput;
	}

	OglShader* OglGraphicsContext::CreateShader(ShaderCompilation* compilation)
	{
		if (compilation->GetStatus() != ShaderCompilationStatus::Success)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Trying to create shader from failed compilation");

			UAVPF_LOG(
				Application,
				Trace,
				"Shader compiler diagnostics:\n%s",
				compilation->GetDiagnostics().data());

			return nullptr;
		}

		auto* actualCompilation = dynamic_cast<OglShaderCompilation*>(compilation);
		GLuint nativeProgram = actualCompilation->ReleaseNativeProgram();

		return new OglShader(nativeProgram, compilation->GetShaderKind());
	}

	void OglGraphicsContext::DestroyShader(Shader* shader)
	{
		delete shader;
	}

	OglTexture2D* OglGraphicsContext::CreateTexture2D(Texture2DParams params)
	{
		return new OglTexture2D(std::move(params));
	}

	void OglGraphicsContext::DestroyTexture2D(Texture2D* texture2D)
	{
		delete texture2D;
	}


	void OglGraphicsContext::SetVertexInput(VertexInput* vertexInput)
	{
		if (nullptr == vertexInput)
		{
			glBindVertexArray(0);
			mBoundVertexInput = nullptr;
			return;
		}

		mBoundVertexInput = dynamic_cast<OglVertexInput*>(vertexInput);
		glBindVertexArray(mBoundVertexInput->GetNativeVertexArray());
	}

	OglVertexInput* OglGraphicsContext::GetVertexInput() const
	{
		return mBoundVertexInput;
	}

	void OglGraphicsContext::SetShader(ShaderKind kind, Shader* shader)
	{
		mShaderPipeline.Set(kind, dynamic_cast<OglShader*>(shader));
	}

	OglShader* OglGraphicsContext::GetShader(ShaderKind kind) const
	{
		return mShaderPipeline.Get(kind);
	}

	void OglGraphicsContext::SetPrimitiveMode(PrimitiveMode mode)
	{
		mPrimitiveMode = mode;
	}

	PrimitiveMode OglGraphicsContext::GetPrimitiveMode() const
	{
		return mPrimitiveMode;
	}

	void OglGraphicsContext::SetViewport(const Viewport& viewport)
	{
		mViewport = viewport;
		SetNativeViewport(viewport);
	}

	const Viewport& OglGraphicsContext::GetViewport() const
	{
		return mViewport;
	}

	OglShaderCompiler* OglGraphicsContext::GetShaderCompiler()
	{
		return &mShaderCompiler;
	} 

	void OglGraphicsContext::Draw(int32_t startVertexIndex, int32_t vertexCount)
	{
		glDrawArrays(
			OglFacts::ConvertPrimitiveModeToNative(mPrimitiveMode),
			startVertexIndex,
			static_cast<GLsizei>(vertexCount));
	}

	void OglGraphicsContext::BindGlobalShaderPipeline()
	{
		glBindProgramPipeline(mShaderPipeline.GetNativeProgramPipeline());
	}

	void OglGraphicsContext::SetNativeViewport(const Viewport& viewport)
	{
		glViewport(
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height);
	}
}

