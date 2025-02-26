#pragma once

#include <string_view>

#include "uavpf/image/tiff_image.h"

namespace uavpf
{
	enum class TiffLoadStatus
	{
		None,
		Success,
		FileNotFound,
		LoaderFailure,
		SizeReadFailure,
		PixelsReadFailure,
	};

	struct TiffImageHandle;

	class TiffLoader
	{
	public:
		TiffImage LoadImageFromFile(std::string_view fileName);
		TiffLoadStatus GetLoadStatus() const;

	private:
		bool ReadImageSize(TiffImageHandle* handle);
		bool ReadImagePixels(TiffImageHandle* handle);

		TiffLoadStatus mLastLoadStatus = TiffLoadStatus::None;
	};
}

