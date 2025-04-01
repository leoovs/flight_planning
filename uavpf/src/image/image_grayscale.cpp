#include "uavpf/image/image_grayscale.h"

#include <cassert>

#include "uavpf/image/rgba_facts.h"

namespace uavpf
{
	ImageGrayscale::ImageGrayscale(TiffImage& accessedImage)
		: mAccessedImage(&accessedImage)
	{}

	TiffImage* ImageGrayscale::GetImage() const
	{
		return mAccessedImage;
	}

	float ImageGrayscale::Get(int32_t x, int32_t y) const
	{
		assert(nullptr != mAccessedImage);

		uint32_t rgba = mAccessedImage->ReadPixelAt(x, y);
		auto [a, b, g, r] = RgbaFacts::DecomposeRgba(rgba);

		float greyscale = (0.2126*r + 0.7152*g + 0.0722*b) / 256.0f;

		return greyscale;
	}

	ImageGrayscale::operator bool() const
	{
		return nullptr != mAccessedImage;
	}
}

