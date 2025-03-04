#pragma once

#include <cinttypes>

#include "graphics/graphics_backend.h"
#include "graphics/graphics_buffer.h"
#include "graphics/graphics_debug_watch.h"
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
	};
}

