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

	GLenum OglFacts::ConvertGraphicsFormatToType(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
			case GraphicsFormat::R8G8_UNORM:
			case GraphicsFormat::R8G8B8_UNORM:
			case GraphicsFormat::R8G8B8A8_UNORM:
				return GL_UNSIGNED_INT;

			case GraphicsFormat::R32_FLOAT:
			case GraphicsFormat::R32G32_FLOAT:
			case GraphicsFormat::R32G32B32_FLOAT:
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return GL_FLOAT;
			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return 0ull;
		}
	}

	GLenum OglFacts::ConvertShaderKindToNative(ShaderKind kind)
	{
		switch (kind)
		{
			case ShaderKind::Vertex:
				return GL_VERTEX_SHADER;
			case ShaderKind::Pixel:
				return GL_FRAGMENT_SHADER;

			default:
				assert(false && "Invalid ShaderKind enum value");
				return 0;
		}
	}

	bool OglFacts::IsGraphicsFormatNormalized(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
			case GraphicsFormat::R8G8_UNORM:
			case GraphicsFormat::R8G8B8_UNORM:
			case GraphicsFormat::R8G8B8A8_UNORM:
				return true;

			case GraphicsFormat::R32_FLOAT:
			case GraphicsFormat::R32G32_FLOAT:
			case GraphicsFormat::R32G32B32_FLOAT:
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return false;

			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return false;
		}
	}
}

