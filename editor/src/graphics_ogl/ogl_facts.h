#pragma once

#include <GL/glew.h>

#include "graphics/graphics_buffer.h"
#include "graphics/graphics_format.h"
#include "graphics/primitive_mode.h"
#include "graphics/shader_kind.h"

namespace editor
{
	class OglFacts
	{
	public:
		static GLbitfield ConvertTargetToNative(GraphicsBufferTarget target);
		static GLbitfield ConvertShaderKindToStageBit(ShaderKind kind);
		static GLenum ConvertGraphicsFormatToType(GraphicsFormat format);
		static GLenum ConvertGraphicsFormatToInternalFormat(GraphicsFormat format);
		static GLenum ConvertGraphicsFormatToFormat(GraphicsFormat format);
		static GLenum ConvertShaderKindToNative(ShaderKind kind);
		static GLenum ConvertPrimitiveModeToNative(PrimitiveMode mode);
		static bool IsGraphicsFormatNormalized(GraphicsFormat format);
	};
}

