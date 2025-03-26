#include "uavpf/image/image_luminance.h"

#include <cassert>

#include "uavpf/image/rgba_facts.h"

namespace uavpf
{
	ImageLuminance::ImageLuminance(TiffImage& accessedImage)
		: mAccessedImage(&accessedImage)
	{}

	TiffImage* ImageLuminance::GetImage() const
	{
		return mAccessedImage;
	}

	float ImageLuminance::FromRgb(int32_t x, int32_t y) const
	{
		assert(nullptr != mAccessedImage);

		uint32_t rgba = mAccessedImage->ReadPixelAt(x, y);
		auto [r, g, b, a] = RgbaFacts::DecomposeRgba(rgba);

		float normR = static_cast<float>(r) / RgbaFacts::cMaxChannelValue; 
		float normG = static_cast<float>(g) / RgbaFacts::cMaxChannelValue; 
		float normB = static_cast<float>(b) / RgbaFacts::cMaxChannelValue; 

		float greyscale = 0.33f * normR + 0.33f * normG + 0.33f * normB;

		return greyscale;
	}

	ImageLuminance::operator bool() const
	{
		return nullptr != mAccessedImage;
	}
}

