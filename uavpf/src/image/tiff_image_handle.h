#pragma once

#include <cstddef>

#include <vector>

#include <tiffio.h>

namespace uavpf
{
	struct TiffImageHandle
	{
		TIFF* NativeTiff = nullptr;
		int32_t Width = 0;
		int32_t Height = 0;
		std::vector<std::uint32_t> RgbaPixels;
	};
}

