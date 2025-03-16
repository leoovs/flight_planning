#include "graphics_ogl/ogl_shader_pipeline.h"

#include <utility>

#include <uavpf/uavpf.h>

#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_shader.h"

namespace editor
{
	OglShaderPipeline::OglShaderPipeline()
	{
		CreateNativeProgramPipeline();
	}

	OglShaderPipeline::OglShaderPipeline(OglShaderPipeline&& other) noexcept
		: mNativeProgramPipeline(std::exchange(other.mNativeProgramPipeline, 0))
	{
	}

	OglShaderPipeline& OglShaderPipeline::operator=(OglShaderPipeline&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		DestroyNativeProgramPipeline();
		mNativeProgramPipeline = std::exchange(other.mNativeProgramPipeline, 0);

		return *this;
	}

	OglShaderPipeline::~OglShaderPipeline()
	{
		DestroyNativeProgramPipeline();
	}

	void OglShaderPipeline::Set(ShaderKind kind, OglShader* shader)
	{
		mBoundShaders.at(size_t(kind)) = shader;
		SetNative(kind, shader);
	}

	OglShader* OglShaderPipeline::Get(ShaderKind kind) const
	{
		return mBoundShaders.at(size_t(kind));
	}

	GLuint OglShaderPipeline::GetNativeProgramPipeline() const
	{
		return mNativeProgramPipeline;
	}

	void OglShaderPipeline::CreateNativeProgramPipeline()
	{
		glCreateProgramPipelines(1, &mNativeProgramPipeline);

		std::string_view debugName = "Global OglShaderPipeline";
		glObjectLabel(
			GL_PROGRAM_PIPELINE,
			mNativeProgramPipeline,
			static_cast<GLsizei>(debugName.length()),
			debugName.data());
	}

	void OglShaderPipeline::DestroyNativeProgramPipeline()
	{
		glDeleteProgramPipelines(1, &mNativeProgramPipeline);
		mNativeProgramPipeline = 0;
	}

	void OglShaderPipeline::SetNative(ShaderKind kind, OglShader* shader)
	{
		GLbitfield nativeStage = OglFacts::ConvertShaderKindToStageBit(kind);
		if (nullptr == shader)
		{
			glUseProgramStages(mNativeProgramPipeline, nativeStage, 0);
			return;
		}

		const ShaderKind bindingKind = shader->GetKind();

		if (bindingKind != kind)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Shader kind mismatch");
			return;
		}

		glUseProgramStages(
			mNativeProgramPipeline,
			nativeStage,
			shader->GetNativeProgram());
	}
}

