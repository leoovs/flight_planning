#pragma once

#include <cinttypes>

namespace editor
{
	class Shader;

	class ShaderConstantBufferReflector
	{
	public:
		virtual ~ShaderConstantBufferReflector() = default;
	};

	class ShaderReflector
	{
	public:
		virtual ~ShaderReflector() = default;

		virtual int32_t GetConstantBufferCount() const = 0;
		virtual ShaderConstantBufferReflector* ReflectConstantBuffer(int32_t index) = 0; 
	};
}

