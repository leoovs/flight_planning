#include "uavpf/terrain/height_map.h"

#include <cassert>

#include "uavpf/image/image_grayscale.h"

namespace uavpf
{
	HeightMap::HeightMap(int32_t width, int32_t depth)
		: mWidth(width)
		, mDepth(depth)
		, mElevation(width * depth)
	{
	}

	HeightMap::HeightMap(int32_t width, int32_t depth, std::vector<float> heightMatrix)
		: mWidth(width)
		, mDepth(depth)
		, mElevation(std::move(heightMatrix))
	{
		assert(width * depth == mElevation.size());
	}
	size_t HeightMap::GetSize() const
	{
		return mElevation.size();
	}

	int32_t HeightMap::GetWidth() const
	{
		return mWidth;
	}

	int32_t HeightMap::GetDepth() const
	{
		return mDepth;
	}

	float HeightMap::GetElevation(int32_t x, int32_t z) const
	{
		int32_t index = z * mWidth + x;
		assert(index < mElevation.size());
		return mElevation.at(index);
	}

	float& HeightMap::At(int32_t x, int32_t z)
	{
		int32_t index = z * mWidth + x;
		assert(index < mElevation.size());
		return mElevation.at(index);
	}

	void HeightMap::Resize(int32_t width, int32_t depth)
	{
		mWidth = width;
		mDepth = depth;
		mElevation.resize(width * depth);
	}

	std::vector<float>::iterator HeightMap::begin()
	{
		return mElevation.begin();
	}

	std::vector<float>::iterator HeightMap::end()
	{
		return mElevation.end();
	}
}

