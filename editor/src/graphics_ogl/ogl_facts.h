#pragma once

#include <GL/glew.h>

#include "graphics/graphics_buffer.h"

namespace editor
{
	class OglFacts
	{
	public:
		static GLbitfield ConvertTargetToNative(GraphicsBufferTarget target);
	};
}

