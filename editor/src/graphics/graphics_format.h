#pragma once

#include <cstddef>

namespace editor
{
	enum class GraphicsFormat
	{
		R8_UNORM,
		R8G8_UNORM,
		R8G8B8_UNORM,
		R8G8B8A8_UNORM,

		R32_FLOAT,
		R32G32_FLOAT,
		R32G32B32_FLOAT,
		R32G32B32A32_FLOAT,

		D32_FLOAT,
	};

	size_t GetGraphicsFormatByteWidth(GraphicsFormat format);
	size_t GetGraphicsFormatElementCount(GraphicsFormat format);
	size_t GetGraphicsFormatAlignment(GraphicsFormat format);
}

