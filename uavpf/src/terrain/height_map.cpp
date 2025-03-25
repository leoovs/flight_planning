#include "uavpf/terrain/height_map.h"

namespace uavpf
{
	HeightMap::HeightMap(int32_t width, int32_t height)
		: mWidth(width)
		, mHeight(height)
		, mHeightMatrix(width * height)
	{
	}

	HeightMap::RowAccessor<float>
	HeightMap::operator[](int32_t x)
	{
		return RowAccessor<float>(
			mHeightMatrix.data(), 
			x,
			mWidth,
			mHeightMatrix.size());
	}

	HeightMap::RowAccessor<const float>
	HeightMap::operator[](int32_t x) const
	{
		return RowAccessor<const float>(
			mHeightMatrix.data(), 
			x,
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
}

