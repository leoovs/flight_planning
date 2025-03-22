#include "graphics_ogl/ogl_graphics_context.h"

#include <cassert>

#include <uavpf/uavpf.h>
#include <GL/glew.h>

#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_framebuffer.h"
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

	OglFramebuffer* OglGraphicsContext::CreateFramebuffer(FramebufferParams params)
	{
		return new OglFramebuffer(std::move(params));
	}

	void OglGraphicsContext::DestroyFramebuffer(Framebuffer* framebuffer)
	{
		if (nullptr == framebuffer)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to delete Framebuffer which is nullptr");
		}
		delete framebuffer;
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

	int32_t OglGraphicsContext::GetMaxConstantBufferSlots() const
	{
		return cMaxConstantBufferSlots;
	}

	void OglGraphicsContext::SetConstantBuffer(
		GraphicsBuffer* constantBuffer,
		int32_t constantBufferSlot)
	{
		assert(constantBufferSlot < cMaxConstantBufferSlots);
		auto actualConstantBuffer = mConstantBuffers.at(constantBufferSlot)
			= dynamic_cast<OglGraphicsBuffer*>(constantBuffer);
		SetNativeUniformBuffer(actualConstantBuffer, constantBufferSlot);
	}

	OglGraphicsBuffer* OglGraphicsContext::GetConstantBuffer(
		int32_t constantBufferSlot) const
	{
		assert(constantBufferSlot < cMaxConstantBufferSlots);
		return mConstantBuffers.at(constantBufferSlot);
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

	void OglGraphicsContext::SetFramebuffer(Framebuffer* framebuffer)
	{
		mFramebuffer = dynamic_cast<OglFramebuffer*>(framebuffer);
		SetNativeFramebuffer(mFramebuffer);
	}

	OglFramebuffer* OglGraphicsContext::GetFramebuffer() const
	{
		return mFramebuffer;
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

	void OglGraphicsContext::ClearColor(
		Framebuffer* framebuffer,
		float r,
		float g,
		float b,
		float a)
	{
		auto actualFramebuffer = dynamic_cast<OglFramebuffer*>(framebuffer);
		GLuint nativeFramebuffer = actualFramebuffer
			? actualFramebuffer->GetNativeFramebuffer()
			: 0;

		float clearColor[]{ r, g, b, a };
		glClearNamedFramebufferfv(nativeFramebuffer, GL_COLOR, 0, clearColor);
	}

	void OglGraphicsContext::ClearDepthStencil(
		Framebuffer* framebuffer,
		float depth,
		uint8_t stencil)
	{
		auto actualFramebuffer = dynamic_cast<OglFramebuffer*>(framebuffer);
		GLuint nativeFramebuffer = actualFramebuffer
			? actualFramebuffer->GetNativeFramebuffer()
			: 0;

		glClearNamedFramebufferfi(
			nativeFramebuffer,
			GL_DEPTH_STENCIL,
			0,
			depth, stencil);
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

	void OglGraphicsContext::SetNativeUniformBuffer(
		OglGraphicsBuffer* constantBuffer,
		int32_t slot)
	{
		if (nullptr == constantBuffer)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, slot, 0);
			return;
		}

		const GraphicsBufferTarget bindingTarget = constantBuffer->GetParams().Target; 
		if (bindingTarget != GraphicsBufferTarget::Constant)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Graphics buffer target mismatch");
			return;
		}

		GLuint nativeUniformBuffer = constantBuffer->GetNativeBuffer();
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, nativeUniformBuffer);
	}

	void OglGraphicsContext::SetNativeFramebuffer(OglFramebuffer* framebuffer)
	{
		glBindFramebuffer(
			GL_FRAMEBUFFER,
			framebuffer
				? framebuffer->GetNativeFramebuffer()
				: 0);
	}
}

