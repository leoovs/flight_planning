#pragma once

#include "graphics/shader_kind.h"

namespace editor
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual ShaderKind GetKind() const = 0;
	};
}

