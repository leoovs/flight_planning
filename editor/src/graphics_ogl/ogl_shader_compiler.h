#pragma once

#include "graphics/shader_compiler.h"

namespace editor
{
	class OglShaderCompiler final : public ShaderCompiler
	{
	public:
		ShaderCompilationResult Compile(
			ShaderKind kind,
			std::string_view sourceCode) override;
	};
}

