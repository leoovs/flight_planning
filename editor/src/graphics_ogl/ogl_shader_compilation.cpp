#include "graphics_ogl/ogl_shader_compilation.h"

#include <utility>

namespace editor
{
	ShaderCompilationStatus OglShaderCompilation::GetStatus() const
	{
		return mStatus;
	}

	std::string_view OglShaderCompilation::GetDiagnostics() const
	{
		return mDiagnostics;
	}

	ShaderKind OglShaderCompilation::GetShaderKind() const
	{
		return mShaderKind;
	}

	void OglShaderCompilation::SetNativeProgram(GLuint nativeProgram)
	{
		mNativeProgram = nativeProgram;
	}

	void OglShaderCompilation::SetNativeShader(GLuint nativeShader)
	{
		mNativeShader = nativeShader;
	}

	void OglShaderCompilation::SetStatus(ShaderCompilationStatus status)
	{
		mStatus = status;
	}

	void OglShaderCompilation::SetDiagnostics(std::string diagnostics)
	{
		mDiagnostics = std::move(diagnostics);
	}

	void OglShaderCompilation::SetShaderKind(ShaderKind kind)
	{
		mShaderKind = kind;
	}

	void OglShaderCompilation::SetShaderSource(std::string_view source)
	{
		mShaderSource = source;
	}

	GLuint OglShaderCompilation::GetNativeProgram() const
	{
		return mNativeProgram;
	}

	GLuint OglShaderCompilation::GetNativeShader() const
	{
		return mNativeShader;
	}

	std::string_view OglShaderCompilation::GetShaderSource() const
	{
		return mShaderSource;
	}

	GLuint OglShaderCompilation::ReleaseNativeProgram()
	{
		return mNativeProgram;
	}

	GLuint OglShaderCompilation::ReleaseNativeShader()
	{
		return std::exchange(mNativeShader, 0);
	}
}

