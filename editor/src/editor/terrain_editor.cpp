#include "editor/terrain_editor.h"

#include "asset/height_map_asset.h"
#include "runtime/rt_module_locator.h"

namespace editor
{
	void TerrainEditor::LoadHeightMap(const std::filesystem::path& path)
	{
		mHeightMap = mAssets->Load(path, AssetKind::HeightMap);
	}

	const uavpf::HeightMap& TerrainEditor::GetHeightMap() const
	{
		return mAssets->Get<HeightMapAsset>(mHeightMap)->GetHeightMap();
	}

	const uavpf::TiffImage& TerrainEditor::GetHeightMapImage() const
	{
		return mAssets->Get<HeightMapAsset>(mHeightMap)->GetImage();
	}

	bool TerrainEditor::IsHeightMapLoaded() const
	{
		return uavpf::cBadID != mHeightMap;
	}
}

