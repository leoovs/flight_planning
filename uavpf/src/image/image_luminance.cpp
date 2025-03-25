#include "uavpf/image/image_luminance.h"

#include <cassert>

#include "uavpf/image/rgba_facts.h"

namespace uavpf
{
	ImageLuminance::ImageLuminance(TiffImage* accessedImage)
		: mAccessedImage(accessedImage)
	{}

	float ImageLuminance::FromRgb(int32_t x, int32_t y) const
	{
		assert(nullptr != mAccessedImage);

		uint32_t rgba = mAccessedImage->ReadPixelAt(x, y);
		auto [r, g, b, a] = RgbaFacts::DecomposeRgba(rgba);

		float normR = static_cast<float>(r) / RgbaFacts::cMaxChannelValue; 
		float normG = static_cast<float>(g) / RgbaFacts::cMaxChannelValue; 
		float normB = static_cast<float>(b) / RgbaFacts::cMaxChannelValue; 

		float luminance = 0.2126f * normR
			+ 0.7152f * normG
			+ 0.0722f * normB;

		return luminance;
	}

	ImageLuminance::operator bool() const
	{
		return nullptr != mAccessedImage;
	}
}

