#pragma once

#include <string_view>

#include <glm/glm.hpp>

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
		virtual void SetUniform(std::string_view name, float x, float y, float z, float w) = 0;
		virtual void SetUniform(std::string_view name, const glm::mat4& v) = 0;

		void SetUniform(std::string_view name, const glm::vec2& v)
		{
			SetUniform(name, v.x, v.y);
		}

		void SetUniform(std::string_view name, const glm::vec3& v)
		{
			SetUniform(name, v.x, v.y, v.z);
		}

		void SetUniform(std::string_view name, const glm::vec4& v) 
		{
			SetUniform(name, v.x, v.y, v.z, v.w);
		}
	};
}

