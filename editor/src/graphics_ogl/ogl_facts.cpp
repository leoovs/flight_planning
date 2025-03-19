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
			case GraphicsBufferTarget::Constant:
				return GL_UNIFORM_BUFFER;

			default:
				assert(false && "Invalid GraphicsBufferTarget enum value");
				return 0;
		}
	}

	GLbitfield OglFacts::ConvertShaderKindToStageBit(ShaderKind kind)
	{
		switch (kind)
		{
			case ShaderKind::Vertex:
				return GL_VERTEX_SHADER_BIT;
			case ShaderKind::Pixel:
				return GL_FRAGMENT_SHADER_BIT;

			default:
				assert(false && "Invalid ShaderKind enum value");
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
				return GL_UNSIGNED_BYTE;

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

	GLenum OglFacts::ConvertGraphicsFormatToInternalFormat(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
				return GL_R8;
			case GraphicsFormat::R8G8_UNORM:
				return GL_RG8;
			case GraphicsFormat::R8G8B8_UNORM:
				return GL_RGB8;
			case GraphicsFormat::R8G8B8A8_UNORM:
				return GL_RGBA8;

			case GraphicsFormat::R32_FLOAT:
				return GL_R32F;
			case GraphicsFormat::R32G32_FLOAT:
				return GL_RG32F;
			case GraphicsFormat::R32G32B32_FLOAT:
				return GL_RGB32F;
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return GL_RGBA32F;

			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return false;
		}
	}

	GLenum OglFacts::ConvertGraphicsFormatToFormat(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
			case GraphicsFormat::R32_FLOAT:
				return GL_R;

			case GraphicsFormat::R8G8_UNORM:
			case GraphicsFormat::R32G32_FLOAT:
				return GL_RG;

			case GraphicsFormat::R8G8B8_UNORM:
			case GraphicsFormat::R32G32B32_FLOAT:
				return GL_RGB;

			case GraphicsFormat::R8G8B8A8_UNORM:
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return GL_RGBA;

			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return 0;
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

	GLenum OglFacts::ConvertPrimitiveModeToNative(PrimitiveMode mode)
	{
		switch (mode)
		{
			case PrimitiveMode::TriangleList:
				return GL_TRIANGLES;
			case PrimitiveMode::LineList:
				return GL_LINES;

			default:
				assert(false && "Unknown PrimitiveMode enum value");
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

