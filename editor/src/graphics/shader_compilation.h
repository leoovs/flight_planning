#pragma once

#include <string_view>

#include "graphics/shader_kind.h"

namespace editor
{
	enum class ShaderCompilationStatus
	{
		None,
		Success,
		Failure,
	};

	class ShaderCompilation
	{
	public:
		virtual ~ShaderCompilation() = default;

		virtual ShaderCompilationStatus GetStatus() const = 0;
		virtual std::string_view GetDiagnostics() const = 0;
		virtual ShaderKind GetShaderKind() const = 0;
	};
}

