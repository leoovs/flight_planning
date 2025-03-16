#pragma once

#include <string>

#include <GL/glew.h>

#include "graphics/shader_compilation.h"

namespace editor
{
	class OglShaderCompilation final : public ShaderCompilation
	{
	public:
		~OglShaderCompilation() override = default;

		ShaderCompilationStatus GetStatus() const override;
		std::string_view GetDiagnostics() const override;
		ShaderKind GetShaderKind() const override;

		void SetNativeProgram(GLuint nativeProgram);
		void SetNativeShader(GLuint nativeShader);
		void SetStatus(ShaderCompilationStatus status);
		void SetDiagnostics(std::string diagnostics);
		void SetShaderKind(ShaderKind kind);
		void SetShaderSource(std::string_view source);

		GLuint GetNativeProgram() const;
		GLuint GetNativeShader() const;
		std::string_view GetShaderSource() const;

		GLuint ReleaseNativeProgram();
		GLuint ReleaseNativeShader();

	private:
		GLuint mNativeProgram = 0;
		GLuint mNativeShader = 0;

		ShaderCompilationStatus mStatus = ShaderCompilationStatus::None;
		std::string mDiagnostics;
		ShaderKind mShaderKind = ShaderKind::Unknown;
		std::string mShaderSource;
	};
}

