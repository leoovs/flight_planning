#pragma once

#include <vector>

#include <GL/glew.h>

#include "graphics/shader_reflector.h"
#include "graphics_ogl/ogl_shader.h"

namespace editor
{
	class OglShaderConstantBufferReflector final : public ShaderConstantBufferReflector
	{
	};

	class OglShaderReflector final : public ShaderReflector
	{
	public:
		OglShaderReflector(OglShader* shader);
		~OglShaderReflector() override = default;

		int32_t GetConstantBufferCount() const override;
		OglShaderConstantBufferReflector* ReflectConstantBuffer(int32_t index) override;

	private:
		void PreReflectConstantBuffers();

		OglShader* mReflectedShader = nullptr;

		GLuint mNativeShaderProgram = 0;
		std::vector<OglShaderConstantBufferReflector> mConstantBuffers;
	};
}

