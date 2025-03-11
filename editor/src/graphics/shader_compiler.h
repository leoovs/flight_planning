#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "graphics/shader_kind.h"

namespace editor
{
	enum class ShaderCompilationFlags
	{
		None = 0x0,
		Success  = 0x1,
		Warnings = 0x2,
		SuccessWithWarnings = Success | Warnings,
	};

	constexpr ShaderCompilationFlags operator|(
		ShaderCompilationFlags left,
		ShaderCompilationFlags right)
	{
		return static_cast<ShaderCompilationFlags>(
			static_cast<std::underlying_type_t<ShaderCompilationFlags>>(left)
			| static_cast<std::underlying_type_t<ShaderCompilationFlags>>(right));
	}

	constexpr ShaderCompilationFlags operator&(
		ShaderCompilationFlags left,
		ShaderCompilationFlags right)
	{
		return static_cast<ShaderCompilationFlags>(
			static_cast<std::underlying_type_t<ShaderCompilationFlags>>(left)
			& static_cast<std::underlying_type_t<ShaderCompilationFlags>>(right));
	}

	class Shader;

	struct ShaderCompilationResult
	{
		ShaderCompilationFlags CompilationFlags = ShaderCompilationFlags::None;
		Shader* ResultingShader = nullptr;
		std::string Diagnostics;
	};

	class ShaderCompiler
	{
	public:
		virtual ShaderCompilationResult Compile(
			ShaderKind kind,
			std::string_view sourceCode) = 0;
	};
}

