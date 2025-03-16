#pragma once

#include <array>

#include <GL/glew.h>

#include "graphics/shader_kind.h"

namespace editor
{
	class OglShader;

	class OglShaderPipeline
	{
	public:
		OglShaderPipeline();
		OglShaderPipeline(OglShaderPipeline&& other) noexcept;

		OglShaderPipeline& operator=(OglShaderPipeline&& other) noexcept;

		~OglShaderPipeline();

		void Set(ShaderKind kind, OglShader* shader);
		OglShader* Get(ShaderKind kind) const;

		GLuint GetNativeProgramPipeline() const;
	
	private:
		void CreateNativeProgramPipeline();
		void DestroyNativeProgramPipeline();

		void SetNative(ShaderKind kind, OglShader* shader);

		GLuint mNativeProgramPipeline = 0;
		std::array<OglShader*, size_t(ShaderKind::Count_)> mBoundShaders;
	};
}

