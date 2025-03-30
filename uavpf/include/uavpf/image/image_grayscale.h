#pragma once

#include "uavpf/image/tiff_image.h"

namespace uavpf
{
	class ImageGrayscale
	{
	public:
		ImageGrayscale() = default;
		ImageGrayscale(TiffImage& accessedImage);

		TiffImage* GetImage() const;
		float Get(int32_t x, int32_t y) const;

		explicit operator bool() const;

	private:
		TiffImage* mAccessedImage = nullptr;
	};
}

