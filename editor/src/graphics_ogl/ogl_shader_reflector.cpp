#include "graphics_ogl/ogl_shader_reflector.h"

namespace editor
{
	OglShaderReflector::OglShaderReflector(OglShader* shader)
		: mReflectedShader(shader)
		, mNativeShaderProgram(shader->GetNativeProgram())
	{
		PreReflectConstantBuffers();
	}

	int32_t OglShaderReflector::GetConstantBufferCount() const
	{
		return static_cast<int32_t>(mConstantBuffers.size());
	}

	OglShaderConstantBufferReflector* OglShaderReflector::ReflectConstantBuffer(
		int32_t index)
	{
		return &mConstantBuffers.at(index);
	}

	void OglShaderReflector::PreReflectConstantBuffers()
	{
		GLint constantBufferCount = 0;
		glGetProgramInterfaceiv(
			mNativeShaderProgram,
			GL_UNIFORM_BLOCK,
			GL_ACTIVE_RESOURCES,
			&constantBufferCount);

		mConstantBuffers.reserve(constantBufferCount);
		for (GLint iConstantBuffer = 0;
			iConstantBuffer < constantBufferCount;
			iConstantBuffer++)
		{
			// TODO: make constant buffer reflections. 
		}
	}
}

