#pragma once

#include <cinttypes>

#include "graphics/graphics_backend.h"
#include "graphics/graphics_buffer.h"
#include "graphics/graphics_debug_watch.h"
#include "graphics/primitive_mode.h"
#include "graphics/shader.h"
#include "graphics/shader_compiler.h"
#include "graphics/shader_reflector.h"
#include "graphics/texture_2d.h"
#include "graphics/vertex_input.h"
#include "graphics/viewport.h"
#include "platform/window.h"

namespace editor
{
	enum class GraphicsContextKind
	{
		Optimal,
		Debug,
	};

	struct GraphicsContextParams
	{
		GraphicsContextKind Kind = GraphicsContextKind::Optimal;
		GraphicsBackend Backend = GraphicsBackend::None;
		Window* OutputWindow = nullptr;
	};

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual const GraphicsContextParams& GetParams() const = 0;

		virtual void Present() = 0;
		virtual void ClearColor(float r, float g, float b, float a) = 0;

		virtual GraphicsDebugWatch* CreateDebugWatch() = 0;
		virtual void DestroyDebugWatch(GraphicsDebugWatch* watch) = 0;

		virtual GraphicsBuffer* CreateBuffer(GraphicsBufferParams params) = 0;
		virtual void DestroyBuffer(GraphicsBuffer* graphicsBuffer) = 0;

		virtual VertexInput* CreateVertexInput(VertexInputParams params) = 0;
		virtual void DestroyVertexInput(VertexInput* vertexInput) = 0;

		virtual Shader* CreateShader(ShaderCompilation* compilation) = 0;
		virtual void DestroyShader(Shader* shader) = 0;

		virtual ShaderReflector* ReflectShader(Shader* shader) = 0;
		virtual void DestroyShaderReflector(ShaderReflector* reflector) = 0;

		virtual Texture2D* CreateTexture2D(Texture2DParams params) = 0;
		virtual void DestroyTexture2D(Texture2D* texture2D) = 0;

		virtual void SetVertexInput(VertexInput* vertexInput) = 0;
		virtual VertexInput* GetVertexInput() const = 0;

		virtual void SetShader(ShaderKind kind, Shader* shader) = 0;
		virtual Shader* GetShader(ShaderKind kind) const = 0;

		virtual int32_t GetMaxConstantBufferSlots() const = 0;
		virtual void SetConstantBuffer(
			GraphicsBuffer* constantBuffer,
			int32_t constantBufferSlot) = 0;
		virtual GraphicsBuffer* GetConstantBuffer(
			int32_t constantBufferSlot) const = 0;

		virtual void SetPrimitiveMode(PrimitiveMode mode) = 0;
		virtual PrimitiveMode GetPrimitiveMode() const = 0;

		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual const Viewport& GetViewport() const = 0;

		virtual ShaderCompiler* GetShaderCompiler() = 0;

		virtual void Draw(int32_t startVertexIndex, int32_t vertexCount) = 0;
	};
}

