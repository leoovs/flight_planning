#pragma once

#include <cinttypes>

#include "graphics/graphics_backend.h"

namespace editor
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual GraphicsBackend GetBackend() = 0;

		virtual void Present() = 0;
	};
}

