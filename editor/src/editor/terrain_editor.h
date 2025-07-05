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

		glm::mat4 GetHeightMapToWorldMatrix() const;

		void SetWorldScale(float scale);
		float GetWorldScale() const;

		void SetHeightScale(float scale);
		float GetHeightScale() const;

		glm::vec3 HeightMapToWorldCoord(glm::ivec2 hmCoord) const;

	private:
		void ScaleToFitUnitSquare();

		AssetStorage* mAssets = RtModuleLocator::Locate<AssetStorage>();
		AssetID mHeightMap = uavpf::cBadID;	
		glm::ivec2 mHeightMapResolution;
		glm::vec3 mHeightMapToTerrainScale{ 1.0f };
		float mWorldScale = 1.0f;
	};
}

