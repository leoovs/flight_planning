#pragma once

#include "uavpf/image/tiff_image.h"

namespace uavpf
{
	class ImageLuminance
	{
	public:
		ImageLuminance() = default;
		ImageLuminance(TiffImage& accessedImage);

		TiffImage* GetImage() const;
		float FromRgb(int32_t x, int32_t y) const;

		explicit operator bool() const;

	private:
		TiffImage* mAccessedImage = nullptr;
	};
}

