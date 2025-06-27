#pragma once

#include <uavpf/image/tiff_image.h>
#include <uavpf/terrain/height_map.h>

#include "asset/asset.h"
#include "asset/asset_importer.h"

namespace editor
{
	class HeightMapAsset final : public Asset
	{
	public:
		HeightMapAsset(
			uavpf::TiffImage heightMapImage,
			uavpf::HeightMap heightMap);
		~HeightMapAsset() override = default;

		AssetKind GetKind() const override;

		const uavpf::TiffImage& GetImage() const;
		const uavpf::HeightMap& GetHeightMap() const;

	private:
		uavpf::TiffImage mHeightMapImage;
		uavpf::HeightMap mHeightMap;
	};

	class HeightMapImporter final : public AssetImporter
	{
	public:
		~HeightMapImporter() override = default;

		AssetKind GetImportedAssetKind() const override;
		std::shared_ptr<Asset> Import(std::filesystem::path assetPath) override;	
		bool IsSuccessful() const override;

	private:
		bool mLoadSuccessful = false;
	};
}

