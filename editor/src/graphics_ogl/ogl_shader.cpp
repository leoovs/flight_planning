#include "graphics_ogl/ogl_shader.h"

#include <cassert>

namespace editor
{
	OglShader::OglShader(GLuint nativeProgram, ShaderKind kind)
		: mNativeProgram(nativeProgram)
		, mKind(kind)
	{
		assert(0 != mNativeProgram);
	}

	OglShader::~OglShader()
	{
		DestroyNativeProgram();
	}

	ShaderKind OglShader::GetKind() const
	{
		return mKind;
	}

	GLuint OglShader::GetNativeProgram() const
	{
		return mNativeProgram;
	}

	void OglShader::DestroyNativeProgram()
	{
		glDeleteProgram(mNativeProgram);
		mNativeProgram = 0;
	}
}

