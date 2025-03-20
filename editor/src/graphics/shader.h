#pragma once

#include <string_view>

#include "graphics/shader_kind.h"

namespace editor
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual ShaderKind GetKind() const = 0;

		virtual void SetUniform(std::string_view name, float x) = 0;
		virtual void SetUniform(std::string_view name, float x, float y) = 0;
		virtual void SetUniform(std::string_view name, float x, float y, float z) = 0;
	};
}

