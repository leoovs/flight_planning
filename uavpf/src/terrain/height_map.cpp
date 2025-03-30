#include "uavpf/terrain/height_map.h"

#include <cassert>

#include "uavpf/image/image_grayscale.h"

namespace uavpf
{
	HeightMap HeightMap::FromLuminance(const ImageGrayscale& luminance)
	{
		assert(luminance);

		int32_t rows = luminance.GetImage()->GetHeight();
		int32_t cols = luminance.GetImage()->GetWidth();
		HeightMap result(rows, cols);

		for (int32_t i = 0; i < cols; i++)
		{
			for (int32_t j = 0; j < rows; j++)
			{
				result[i][j] = luminance.Get(j, i);
			}
		}

		return result;
	}

	HeightMap::HeightMap(int32_t width, int32_t height)
		: mWidth(width)
		, mHeight(height)
		, mHeightMatrix(width * height)
	{
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
}

