#pragma once

#include <cinttypes>

#include <string>

#include "graphics/graphics_format.h"
#include "graphics/subresource.h"

namespace editor
{
	struct Texture2DParams
	{
		std::string DebugName = "Unnamed Texture2D";
		int32_t Width = 8;
		int32_t Height = 8;
		int32_t MipLevelCount = 1;
		GraphicsFormat Format = GraphicsFormat::R8_UNORM;
	};

	class Texture2D
	{
	public:
		virtual ~Texture2D() = default;

		virtual const Texture2DParams& GetParams() const = 0;

		virtual void SetData(
			const SubresourceRegion& region,
			const void* srcData) = 0;
	};
}
