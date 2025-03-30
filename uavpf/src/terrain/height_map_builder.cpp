#include "uavpf/terrain/height_map_builder.h"

#include <algorithm>

namespace uavpf
{
	HeightMapBuilder& HeightMapBuilder::SetGrayscale(ImageGrayscale grayscale)
	{
		mGrayscale = grayscale;
		return *this;
	}

	HeightMapBuilder& HeightMapBuilder::SetRasterSpace(RasterSpace rasterSpace)
	{
		mRasterSpace = rasterSpace;
		return *this;
	}

	HeightMap HeightMapBuilder::Build()
	{
		switch (mRasterSpace)
		{
			case RasterSpace::RasterIsArea:
				GenerateHeightFromAreas();
				break;
			case RasterSpace::RasterIsPoint:
				GenerateHeightFromPoints();
				break;
		}

		HeightMap result = std::move(mHeightMap);
		return result;
	}

	void HeightMapBuilder::GenerateHeightFromAreas()
	{
		int32_t width = mGrayscale.GetImage()->GetWidth();
		int32_t height = mGrayscale.GetImage()->GetHeight();

		mHeightMap = HeightMap(width * 2, height * 2);

		for (int32_t i = 0; i < width; i++)
		{
			int32_t col = i * 2;
			for (int32_t j = 0; j < height; j++)
			{
				int32_t row = j * 2;
				float elevation = mGrayscale.Get(i, j);

				mHeightMap[row+0][col+0] = elevation;
				mHeightMap[row+1][col+0] = elevation;
				mHeightMap[row+0][col+1] = elevation;
				mHeightMap[row+1][col+1] = elevation;
			}
		}
	}

	void HeightMapBuilder::GenerateHeightFromPoints()
	{
		int32_t rows = mGrayscale.GetImage()->GetHeight();
		int32_t cols = mGrayscale.GetImage()->GetWidth();

		mHeightMap = HeightMap(cols, rows);

		for (int32_t i = 0; i < cols; i++)
		{
			for (int32_t j = 0; j < rows; j++)
			{
				mHeightMap[j][i] = mGrayscale.Get(i, j);
			}
		}
	}

	void HeightMapBuilder::Minimize()
	{
		float minHeight = *std::min_element(mHeightMap.begin(), mHeightMap.end());
		std::for_each(mHeightMap.begin(), mHeightMap.end(),
			[&minHeight](float& height)
			{
				height -= minHeight;
			}
		);
	}

}

