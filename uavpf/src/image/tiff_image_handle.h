#pragma once

#include <cstddef>

#include <memory>
#include <vector>

#include <tiffio.h>

namespace uavpf
{
	struct TiffDeleter
	{
		void operator()(TIFF* tiff)
		{
			TIFFClose(tiff);
		}
	};

	using TiffUniquePtr = std::unique_ptr<TIFF, TiffDeleter>;

	struct TiffImageHandle
	{
		TiffUniquePtr NativeTiff;
		int32_t Width = 0;
		int32_t Height = 0;
		std::vector<std::uint32_t> RgbaPixels;
	};
}

