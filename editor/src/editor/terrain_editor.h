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
		glm::ivec2 GetHeightMapResolution() const;
		bool IsHeightMapLoaded() const;

		glm::vec3 GetTerrainScale() const;
		void SetTerrainScale(const glm::vec3& scale);
		void ScaleToFitUnitSquare();
		glm::mat4 GetTerrainScaleAsMatrix() const;

		glm::vec3 HeightMapToWorldCoord(glm::ivec2 hmCoord) const;

	private:
		AssetStorage* mAssets = RtModuleLocator::Locate<AssetStorage>();
		AssetID mHeightMap = uavpf::cBadID;	
		glm::ivec2 mHeightMapResolution;
		glm::vec3 mTerrainScale{ 1.0f };
	};
}

