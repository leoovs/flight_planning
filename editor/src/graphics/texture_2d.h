#pragma once

#include "string"

#include "graphics/graphics_format.h"

namespace editor
{
	struct Texture2DParams
	{
		std::string textureName;
		int width;
		int height;
		int mipLevels;
		GraphicsFormat format;
	};

	class Texture2D
	{
	public:
		virtual ~Texture2D() = default;

		virtual void SetData(const void* data) = 0;
	};
}
