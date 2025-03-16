#pragma once

#include <memory>

#include "graphics/graphics_context.h"
#include "graphics_ogl/ogl_graphics_buffer.h"
#include "graphics_ogl/ogl_graphics_debug_watch.h"
#include "graphics_ogl/ogl_provider.h"
#include "graphics_ogl/ogl_shader.h"
#include "graphics_ogl/ogl_shader_compiler.h"
#include "graphics_ogl/ogl_shader_pipeline.h"
#include "graphics_ogl/ogl_vertex_input.h"

namespace editor
{
	class OglGraphicsContext final : public GraphicsContext
	{
	public:
		OglGraphicsContext(
			GraphicsContextParams params,
			std::unique_ptr<OglProvider> provider);

		virtual const GraphicsContextParams& GetParams() const override;

		void Present() override;
		void ClearColor(float r, float g, float b, float a) override;

		OglGraphicsDebugWatch* CreateDebugWatch() override;
		void DestroyDebugWatch(GraphicsDebugWatch* debugWatch) override;

		OglGraphicsBuffer* CreateBuffer(GraphicsBufferParams params) override;
		void DestroyBuffer(GraphicsBuffer* graphicsBuffer) override;

		OglVertexInput* CreateVertexInput(VertexInputParams params) override;
		void DestroyVertexInput(VertexInput* vertexInput) override;

		OglShader* CreateShader(ShaderCompilation* compilation) override;
		void DestroyShader(Shader* shader) override;

		void SetVertexInput(VertexInput* vertexInput) override;
		OglVertexInput* GetVertexInput() const override;

		void SetShader(ShaderKind kind, Shader* shader) override;
		OglShader* GetShader(ShaderKind kind) const override;

		void SetPrimitiveMode(PrimitiveMode mode) override;
		PrimitiveMode GetPrimitiveMode() const override;

		void SetViewport(const Viewport& viewport) override;
		const Viewport& GetViewport() const override;

		OglShaderCompiler* GetShaderCompiler() override;

		void Draw(int32_t startVertexIndex, int32_t vertexCount) override;

	private:
		void BindGlobalShaderPipeline();
		void SetNativeViewport(const Viewport& viewport);

		GraphicsContextParams mParams;
		std::unique_ptr<OglProvider> mProvider;

		OglGraphicsDebugWatch* mRegisteredDebugWatch = nullptr;
		OglVertexInput* mBoundVertexInput = nullptr;
		OglShaderCompiler mShaderCompiler;
		OglShaderPipeline mShaderPipeline;
		PrimitiveMode mPrimitiveMode = PrimitiveMode::LineList;
		Viewport mViewport;
	};
}

