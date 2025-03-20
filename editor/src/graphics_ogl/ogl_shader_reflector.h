#pragma once

#include "graphics/shader_reflector.h"
#include "graphics_ogl/ogl_shader.h"

namespace editor
{
	class OglShaderReflector final : public ShaderReflector
	{
	public:
		OglShaderReflector(OglShader* shader);
		~OglShaderReflector() override = default;

	private:
		OglShader* mReflectedShader = nullptr;
		GLuint mNativeShaderProgram = 0;
	};
}

