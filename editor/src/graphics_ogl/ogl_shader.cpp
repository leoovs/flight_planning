#include "graphics_ogl/ogl_shader.h"

#include <cassert>

#include <glm/gtc/type_ptr.hpp>

namespace editor
{
	OglShaderUniformLocator::OglShaderUniformLocator(GLuint nativeShaderProgram)
		: mNativeShaderProgram(nativeShaderProgram)
	{
	}

	GLuint OglShaderUniformLocator::GetUniformLocation(std::string_view name)
	{
		std::string key(name);
		if (mLocationsCache.count(key))
		{
			return mLocationsCache.at(key);
		}
		return mLocationsCache[key] = glGetUniformLocation(
			mNativeShaderProgram,
			name.data());
	}

	OglShader::OglShader(GLuint nativeProgram, ShaderKind kind)
		: mNativeProgram(nativeProgram)
		, mKind(kind)
		, mUniformLocator(nativeProgram)
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

	void OglShader::SetUniform(std::string_view name, int x)
	{
		glProgramUniform1i(
			mNativeProgram,
			mUniformLocator.GetUniformLocation(name),
			x);
	}

	void OglShader::SetUniform(std::string_view name, float x)
	{
		glProgramUniform1f(
			mNativeProgram, 
			mUniformLocator.GetUniformLocation(name),
			x);
	}

	void OglShader::SetUniform(std::string_view name, float x, float y)
	{
		glProgramUniform2f(
			mNativeProgram, 
			mUniformLocator.GetUniformLocation(name),
			x,
			y);
	}

	void OglShader::SetUniform(std::string_view name, float x, float y, float z)
	{
		glProgramUniform3f(
			mNativeProgram, 
			mUniformLocator.GetUniformLocation(name),
			x,
			y,
			z);
	}

	void OglShader::SetUniform(std::string_view name, float x, float y, float z, float w)
	{
		glProgramUniform4f(
			mNativeProgram,
			mUniformLocator.GetUniformLocation(name),
			x,
			y,
			z,
			w);
	}

	void OglShader::SetUniform(std::string_view name, const glm::mat4& v)
	{
		glProgramUniformMatrix4fv(
			mNativeProgram,
			mUniformLocator.GetUniformLocation(name),
			1,
			GL_FALSE,
			glm::value_ptr(v));
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

