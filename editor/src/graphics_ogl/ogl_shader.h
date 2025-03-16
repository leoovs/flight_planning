#pragma once

#include <GL/glew.h>

#include "graphics/shader.h"

namespace editor
{
	class OglShader final : public Shader
	{
	public:
		OglShader(GLuint nativeProgram, ShaderKind kind);
		~OglShader() override;

		ShaderKind GetKind() const override;

		GLuint GetNativeProgram() const;

	private:
		void DestroyNativeProgram();

		GLuint mNativeProgram = 0;
		ShaderKind mKind = ShaderKind::Unknown;
	};
}

