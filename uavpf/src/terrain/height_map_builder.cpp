#include "uavpf/terrain/height_map_builder.h"

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

				mHeightMap.At(col, row) = elevation;
				mHeightMap.At(col, row) = elevation;
				mHeightMap.At(col, row) = elevation;
				mHeightMap.At(col, row) = elevation;
			}
		}
	}

	void HeightMapBuilder::GenerateHeightFromPoints()
	{
		int32_t width = mGrayscale.GetImage()->GetWidth();
		int32_t depth = mGrayscale.GetImage()->GetHeight();

		mHeightMap = HeightMap(width, depth);

		for (int32_t i = 0; i < width; i++)
		{
			for (int32_t j = 0; j < depth; j++)
			{
				mHeightMap.At(i, depth - j - 1) = mGrayscale.Get(i, j);
			}
		}
	}
}

