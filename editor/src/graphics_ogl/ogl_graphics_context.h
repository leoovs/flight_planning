#pragma once

#include <memory>

#include "graphics/graphics_context.h"
#include "graphics/texture_2d.h"
#include "graphics_ogl/ogl_graphics_buffer.h"
#include "graphics_ogl/ogl_framebuffer.h"
#include "graphics_ogl/ogl_graphics_debug_watch.h"
#include "graphics_ogl/ogl_provider.h"
#include "graphics_ogl/ogl_shader.h"
#include "graphics_ogl/ogl_shader_compiler.h"
#include "graphics_ogl/ogl_shader_pipeline.h"
#include "graphics_ogl/ogl_texture_2d.h"
#include "graphics_ogl/ogl_vertex_input.h"

namespace editor
{
	class OglGraphicsContext final : public GraphicsContext
	{
	public:
		static constexpr int32_t cMaxConstantBufferSlots = 16;

		OglGraphicsContext(
			GraphicsContextParams params,
			std::unique_ptr<OglProvider> provider);

		const GraphicsContextParams& GetParams() const override;

		void Present() override;

		OglGraphicsDebugWatch* CreateDebugWatch() override;
		void DestroyDebugWatch(GraphicsDebugWatch* debugWatch) override;

		OglGraphicsBuffer* CreateBuffer(GraphicsBufferParams params) override;
		void DestroyBuffer(GraphicsBuffer* graphicsBuffer) override;

		OglVertexInput* CreateVertexInput(VertexInputParams params) override;
		void DestroyVertexInput(VertexInput* vertexInput) override;

		OglShader* CreateShader(ShaderCompilation* compilation) override;
		void DestroyShader(Shader* shader) override;

		OglTexture2D* CreateTexture2D(Texture2DParams params) override;
		void DestroyTexture2D(Texture2D* texture2D) override;

		OglFramebuffer* CreateFramebuffer(FramebufferParams params) override;
		void DestroyFramebuffer(Framebuffer* framebuffer) override;

		void SetVertexInput(VertexInput* vertexInput) override;
		OglVertexInput* GetVertexInput() const override;

		void SetShader(ShaderKind kind, Shader* shader) override;
		OglShader* GetShader(ShaderKind kind) const override;

		int32_t GetMaxConstantBufferSlots() const override;
		void SetConstantBuffer(
			GraphicsBuffer* constantBuffer,
			int32_t constantBufferSlot) override;
		OglGraphicsBuffer* GetConstantBuffer(
			int32_t constantBufferSlot) const override;

		void SetPrimitiveMode(PrimitiveMode mode) override;
		PrimitiveMode GetPrimitiveMode() const override;

		void SetViewport(const Viewport& viewport) override;
		const Viewport& GetViewport() const override;

		void SetFramebuffer(Framebuffer* framebuffer) override;
		OglFramebuffer* GetFramebuffer() const override;

		OglShaderCompiler* GetShaderCompiler() override;

		void Draw(int32_t startVertexIndex, int32_t vertexCount) override;

		void ClearColor(
			Framebuffer* framebuffer,
			float r,
			float g,
			float b,
			float a) override;
		void ClearDepthStencil(
			Framebuffer* framebuffer,
			float depth,
			uint8_t stencil) override;

	private:
		void BindGlobalShaderPipeline();
		void SetNativeViewport(const Viewport& viewport);
		void SetNativeUniformBuffer(OglGraphicsBuffer* constantBuffer, int32_t slot);
		void SetNativeFramebuffer(OglFramebuffer* framebuffer);

		GraphicsContextParams mParams;
		std::unique_ptr<OglProvider> mProvider;

		OglGraphicsDebugWatch* mRegisteredDebugWatch = nullptr;
		OglVertexInput* mBoundVertexInput = nullptr;
		OglShaderCompiler mShaderCompiler;
		OglShaderPipeline mShaderPipeline;
		std::array<OglGraphicsBuffer*, cMaxConstantBufferSlots> mConstantBuffers;
		PrimitiveMode mPrimitiveMode = PrimitiveMode::LineList;
		Viewport mViewport;
		OglFramebuffer* mFramebuffer = nullptr;
	};
}

