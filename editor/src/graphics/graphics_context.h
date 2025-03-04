#pragma once

#include <cinttypes>

#include "graphics/graphics_backend.h"
#include "graphics/graphics_buffer.h"
#include "platform/window.h"

namespace editor
{
	struct GraphicsContextParams
	{
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

		virtual GraphicsBuffer* CreateGraphicsBuffer(GraphicsBufferParams params) = 0;
		virtual void DestroyGraphicsBuffer(GraphicsBuffer* graphicsBuffer) = 0;
	};
}

