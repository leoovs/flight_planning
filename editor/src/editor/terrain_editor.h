#pragma once

#include <filesystem>

#include <uavpf/image/tiff_image.h>
#include <uavpf/terrain/height_map.h>

#include "asset/asset_storage.h"
#include "runtime/rt_module_locator.h"

namespace editor
{
	class TerrainEditor
	{
	public:
		void LoadHeightMap(const std::filesystem::path& path);

		const uavpf::HeightMap& GetHeightMap() const;
		const uavpf::TiffImage& GetHeightMapImage() const;

		bool IsHeightMapLoaded() const;

	private:
		AssetStorage* mAssets = RtModuleLocator::Locate<AssetStorage>();
		AssetID mHeightMap = uavpf::cBadID;	
	};
}

