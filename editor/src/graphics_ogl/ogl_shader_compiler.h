#pragma once

#include "graphics/shader_compiler.h"
#include "graphics_ogl/ogl_shader_compilation.h"

namespace editor
{
	class OglShaderCompiler final : public ShaderCompiler
	{
	public:
		~OglShaderCompiler() override = default;

		ShaderCompilation* Compile(
			ShaderKind kind,
			std::string_view sourceCode) override;

		void DestroyCompilation(ShaderCompilation* compilation) override;

	private:
		void CreateNativeProgram(OglShaderCompilation* compilation);
		void DestroyNativeProgram(OglShaderCompilation* compilation);

		void CreateNativeShader(OglShaderCompilation* compilation);
		void DestroyNativeShader(OglShaderCompilation* compilation);

		void CompileNativeShader(OglShaderCompilation* compilation);
		void LinkNativeProgram(OglShaderCompilation* compilation);
		void QueryDiagnostics(OglShaderCompilation* compilation);
	};
}

