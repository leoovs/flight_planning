#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>

#include "graphics/shader.h"

namespace editor
{
	class OglShaderUniformLocator
	{
	public:
		OglShaderUniformLocator(GLuint nativeShaderProgram);

		GLuint GetUniformLocation(std::string_view name);

	private:
		GLuint mNativeShaderProgram = 0;
		std::unordered_map<std::string, GLuint> mLocationsCache;
	};

	class OglShader final : public Shader
	{
	public:
		OglShader(GLuint nativeProgram, ShaderKind kind);
		~OglShader() override;

		ShaderKind GetKind() const override;

		void SetUniform(std::string_view name, float x) override;
		void SetUniform(std::string_view name, float x, float y) override;
		void SetUniform(std::string_view name, float x, float y, float z) override;
		void SetUniform(std::string_view name, float x, float y, float z, float w) override;
		void SetUniform(std::string_view name, const glm::mat4& v) override;

		GLuint GetNativeProgram() const;

	private:
		void DestroyNativeProgram();

		GLuint mNativeProgram = 0;
		ShaderKind mKind = ShaderKind::Unknown;
		OglShaderUniformLocator mUniformLocator;
	};
}

