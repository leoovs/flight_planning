#include "graphics_ogl/ogl_facts.h"

#include <cassert>

namespace editor
{
	GLbitfield OglFacts::ConvertTargetToNative(GraphicsBufferTarget target)
	{
		switch (target)
		{
			case GraphicsBufferTarget::Vertex:	
				return GL_ARRAY_BUFFER;
			case GraphicsBufferTarget::Index:	
				return GL_ELEMENT_ARRAY_BUFFER;
			default:
				assert(false && "Invalid GraphicsBufferTarget enum value");
				return 0;
		}
	}
}

