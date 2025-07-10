#include "asset/height_map_asset.h"
#include "uavpf/terrain/height_map_builder.h"

#include <uavpf/image/tiff_loader.h>

namespace editor
{
	HeightMapAsset::HeightMapAsset(
		uavpf::TiffImage heightMapImage,
		uavpf::HeightMap heightMap)
		: mHeightMapImage(std::move(heightMapImage))
		, mHeightMap(std::move(heightMap))
	{}

	AssetKind HeightMapAsset::GetKind() const
	{
		return AssetKind::HeightMap;
	}

	const uavpf::TiffImage& HeightMapAsset::GetImage() const
	{
		return mHeightMapImage;
	}

	const uavpf::HeightMap& HeightMapAsset::GetHeightMap() const
	{
		return mHeightMap;
	}

	AssetKind HeightMapImporter::GetImportedAssetKind() const
	{
		return AssetKind::HeightMap;
	}

	std::shared_ptr<Asset> HeightMapImporter::Import(std::filesystem::path assetPath)
	{
		uavpf::TiffLoader tiffLoader;
		uavpf::TiffImage heightMapImage = tiffLoader.LoadImageFromFile(assetPath);

		mLoadSuccessful = tiffLoader.GetLoadStatus() == uavpf::TiffLoadStatus::Success;
		if (!mLoadSuccessful)
		{
			return nullptr;
		}

		uavpf::HeightMap heightMap = uavpf::HeightMapBuilder()
			.SetGrayscale(uavpf::ImageGrayscale(heightMapImage))
			.SetRasterSpace(uavpf::RasterSpace::RasterIsPoint)
			.Build();

		return std::make_shared<HeightMapAsset>(
			std::move(heightMapImage),
			std::move(heightMap));
	}

	bool HeightMapImporter::IsSuccessful() const
	{
		return mLoadSuccessful;
	}
}

