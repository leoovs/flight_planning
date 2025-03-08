#include "graphics/graphics_format.h"

#include <cassert>
#include <cinttypes>

namespace editor
{
	size_t GetGraphicsFormatByteWidth(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
				return sizeof(uint8_t[1]);
			case GraphicsFormat::R8G8_UNORM:
				return sizeof(uint8_t[2]);
			case GraphicsFormat::R8G8B8_UNORM:
				return sizeof(uint8_t[3]);
			case GraphicsFormat::R8G8B8A8_UNORM:
				return sizeof(uint8_t[4]);

			case GraphicsFormat::R32_FLOAT:
				return sizeof(float[1]);
			case GraphicsFormat::R32G32_FLOAT:
				return sizeof(float[2]);
			case GraphicsFormat::R32G32B32_FLOAT:
				return sizeof(float[3]);
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return sizeof(float[4]);
			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return 0;
		}
	}

	size_t GetGraphicsFormatElementCount(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
			case GraphicsFormat::R32_FLOAT:
				return 1;
			case GraphicsFormat::R8G8_UNORM:
			case GraphicsFormat::R32G32_FLOAT:
				return 2;
			case GraphicsFormat::R8G8B8_UNORM:
			case GraphicsFormat::R32G32B32_FLOAT:
				return 3;
			case GraphicsFormat::R8G8B8A8_UNORM:
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return 4;
			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return 0;
		}
	}

	size_t GetGraphicsFormatAlignment(GraphicsFormat format)
	{
		switch (format)
		{
			case GraphicsFormat::R8_UNORM:
			case GraphicsFormat::R8G8_UNORM:
			case GraphicsFormat::R8G8B8_UNORM:
			case GraphicsFormat::R8G8B8A8_UNORM:
				return alignof(uint8_t);

			case GraphicsFormat::R32_FLOAT:
			case GraphicsFormat::R32G32_FLOAT:
			case GraphicsFormat::R32G32B32_FLOAT:
			case GraphicsFormat::R32G32B32A32_FLOAT:
				return alignof(float);
			default:
				assert(false && "Invalid GraphicsFormat enum value");
				return 0;
		}
	}
}

