#include "graphics_ogl/ogl_shader_compiler.h"

#include <vector>
#include <sstream>

#include "graphics/shader_compilation.h"
#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_shader_compilation.h"

namespace editor
{
	ShaderCompilation* OglShaderCompiler::Compile(
		ShaderKind kind,
		std::string_view sourceCode)
	{
		auto* compilation = new OglShaderCompilation();

		compilation->SetShaderKind(kind);
		compilation->SetShaderSource(sourceCode);

		CreateNativeProgram(compilation);
		CreateNativeShader(compilation);
		CompileNativeShader(compilation);
		LinkNativeProgram(compilation);
		QueryDiagnostics(compilation);

		return compilation;
	}

	void OglShaderCompiler::DestroyCompilation(ShaderCompilation* compilation)
	{
		auto* actualCompilation = dynamic_cast<OglShaderCompilation*>(compilation);
		DestroyNativeProgram(actualCompilation);
		DestroyNativeShader(actualCompilation);
	}

	void OglShaderCompiler::CreateNativeProgram(OglShaderCompilation* compilation)
	{
		GLuint nativeProgram = glCreateProgram();
		glProgramParameteri(nativeProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);

		compilation->SetNativeProgram(nativeProgram);
	}

	void OglShaderCompiler::DestroyNativeProgram(OglShaderCompilation* compilation)
	{
		GLuint nativeProgram = compilation->ReleaseNativeProgram();
		if (0 != nativeProgram)
		{
			glDeleteProgram(nativeProgram);
			nativeProgram = 0;
		}
	}

	void OglShaderCompiler::CreateNativeShader(OglShaderCompilation* compilation)
	{
		GLuint nativeShader = glCreateShader(
			OglFacts::ConvertShaderKindToNative(
				compilation->GetShaderKind()));

		compilation->SetNativeShader(nativeShader);
	}

	void OglShaderCompiler::DestroyNativeShader(OglShaderCompilation* compilation)
	{
		GLuint nativeShader = compilation->ReleaseNativeShader();

		if (0 != nativeShader)
		{
			glDeleteShader(nativeShader);
			nativeShader = 0;
		}
	}

	void OglShaderCompiler::CompileNativeShader(OglShaderCompilation* compilation)
	{
		GLuint nativeShader = compilation->GetNativeShader();
		std::string_view source = compilation->GetShaderSource();

		const GLchar* nativeSource[]{ source.data() }; 
		glShaderSource(nativeShader, std::size(nativeSource), nativeSource, nullptr);
		glCompileShader(nativeShader);
	}

	void OglShaderCompiler::LinkNativeProgram(OglShaderCompilation* compilation)
	{
		GLuint nativeShader = compilation->GetNativeShader();
		GLuint nativeProgram = compilation->GetNativeProgram();

		GLint isCompiled = GL_FALSE;
		glGetShaderiv(nativeShader, GL_COMPILE_STATUS, &isCompiled);

		if (!isCompiled)
		{
			compilation->SetStatus(ShaderCompilationStatus::Failure);
			return;
		}

		glAttachShader(nativeProgram, nativeShader);
		glLinkProgram(nativeProgram);
		glDetachShader(nativeProgram, nativeShader);

		GLint isLinked = GL_FALSE;
		glGetProgramiv(nativeProgram, GL_LINK_STATUS, &isLinked);

		compilation->SetStatus(
			isLinked
			? ShaderCompilationStatus::Success
			: ShaderCompilationStatus::Failure);
	}

	void OglShaderCompiler::QueryDiagnostics(OglShaderCompilation* compilation)
	{
		GLuint nativeShader = compilation->GetNativeShader();
		GLuint nativeProgram = compilation->GetNativeProgram();

		GLint diagnosticsLogLength = 0;
		std::vector<GLchar> diagnosticsBuffer;
		std::stringstream diagnostics;

		glGetShaderiv(nativeShader, GL_INFO_LOG_LENGTH, &diagnosticsLogLength);
		diagnosticsBuffer.resize(diagnosticsLogLength);
		glGetShaderInfoLog(
			nativeShader, 
			static_cast<GLsizei>(diagnosticsBuffer.size()),
			nullptr,
			diagnosticsBuffer.data());

		diagnostics << "Compiler Errors:\n"
			<< diagnosticsBuffer.data();

		glGetProgramiv(nativeProgram, GL_INFO_LOG_LENGTH, &diagnosticsLogLength);
		diagnosticsBuffer.resize(diagnosticsLogLength);
		glGetProgramInfoLog(
			nativeProgram, 
			static_cast<GLsizei>(diagnosticsBuffer.size()),
			nullptr,
			diagnosticsBuffer.data());

		diagnostics << "\nLinker Errors:\n"
			<< diagnosticsBuffer.data();

		compilation->SetDiagnostics(diagnostics.str());
	}
}
