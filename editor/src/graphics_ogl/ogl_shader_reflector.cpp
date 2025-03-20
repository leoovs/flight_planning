#include "graphics_ogl/ogl_shader_reflector.h"

namespace editor
{
	OglShaderReflector::OglShaderReflector(OglShader* shader)
		: mReflectedShader(shader)
		, mNativeShaderProgram(shader->GetNativeProgram())
	{
	}
}

