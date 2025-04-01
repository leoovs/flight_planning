#pragma once

#include "uavpf/terrain/height_map.h"
#include "uavpf/image/image_grayscale.h"

namespace uavpf
{
	enum class RasterSpace
	{
		RasterIsArea,
		RasterIsPoint,
	};

	class HeightMapBuilder
	{
	public:
		HeightMapBuilder& SetGrayscale(ImageGrayscale grayscale);
		HeightMapBuilder& SetRasterSpace(RasterSpace rasterSpace);

		HeightMap Build();

	private:
		void GenerateHeightFromAreas();
		void GenerateHeightFromPoints();

		ImageGrayscale mGrayscale;	
		RasterSpace mRasterSpace = RasterSpace::RasterIsArea;
		HeightMap mHeightMap;
	};
}

