#include "uavpf/terrain/height_map.h"

#include <cassert>

#include "uavpf/image/image_grayscale.h"

namespace uavpf
{
	HeightMap::HeightMap(int32_t width, int32_t height)
		: mWidth(width)
		, mHeight(height)
		, mHeightMatrix(width * height)
	{
	}

	HeightMap::HeightMap(int32_t width, int32_t height, std::vector<float> heightMatrix)
		: mWidth(width)
		, mHeight(height)
		, mHeightMatrix(std::move(heightMatrix))
	{
		assert(width * height == mHeightMatrix.size());
	}

	HeightMap::RowAccessor<float>
	HeightMap::operator[](int32_t row)
	{
		return RowAccessor<float>(
			mHeightMatrix.data(), 
			row,
			mWidth,
			mHeightMatrix.size());
	}

	HeightMap::RowAccessor<const float>
	HeightMap::operator[](int32_t row) const
	{
		return RowAccessor<const float>(
			mHeightMatrix.data(), 
			row,
			mWidth,
			mHeightMatrix.size());
	}

	size_t HeightMap::GetSize() const
	{
		return mHeightMatrix.size();
	}

	int32_t HeightMap::GetWidth() const
	{
		return mWidth;
	}

	int32_t HeightMap::GetHeight() const
	{
		return mHeight;
	}

	void HeightMap::Resize(int32_t width, int32_t height)
	{
		mWidth = width;
		mHeight = height;
		mHeightMatrix.resize(width * height);
	}

	std::vector<float>::iterator HeightMap::begin()
	{
		return mHeightMatrix.begin();
	}

	std::vector<float>::iterator HeightMap::end()
	{
		return mHeightMatrix.end();
	}
}

