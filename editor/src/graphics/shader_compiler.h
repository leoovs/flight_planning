#pragma once

#include "graphics/shader_compilation.h"
#include "graphics/shader_kind.h"

namespace editor
{
	class ShaderCompiler
	{
	public:
		virtual ~ShaderCompiler() = default;

		virtual ShaderCompilation* Compile(
			ShaderKind kind,
			std::string_view sourceCode) = 0;
		virtual void DestroyCompilation(ShaderCompilation* compilation) = 0;
	};
}

