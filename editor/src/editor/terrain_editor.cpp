#include "editor/terrain_editor.h"

#include <glm/ext/matrix_transform.hpp>

#include "asset/height_map_asset.h"

#include "runtime/rt_module_locator.h"

namespace editor
{
	void TerrainEditor::LoadHeightMap(const std::filesystem::path& path)
	{
		mHeightMap = mAssets->Load(path, AssetKind::HeightMap);

		auto hm = mAssets->Get<HeightMapAsset>(mHeightMap)->GetHeightMap();
		mHeightMapResolution = {
			hm.GetWidth(),
			hm.GetDepth(),
		};
	}

	const uavpf::HeightMap& TerrainEditor::GetHeightMap() const
	{
		return mAssets->Get<HeightMapAsset>(mHeightMap)->GetHeightMap();
	}

	const uavpf::TiffImage& TerrainEditor::GetHeightMapImage() const
	{
		return mAssets->Get<HeightMapAsset>(mHeightMap)->GetImage();
	}

	glm::ivec2 TerrainEditor::GetHeightMapResolution() const
	{
		return mHeightMapResolution;
	}

	bool TerrainEditor::IsHeightMapLoaded() const
	{
		return uavpf::cBadID != mHeightMap;
	}

	glm::vec3 TerrainEditor::GetTerrainScale() const
	{
		return mTerrainScale;
	}

	void TerrainEditor::SetTerrainScale(const glm::vec3& scale)
	{
		mTerrainScale = scale;
	}

	void TerrainEditor::ScaleToFitUnitSquare()
	{
		SetTerrainScale({
			1.0f / mHeightMapResolution.x,
			1.0f,
			1.0f / mHeightMapResolution.y
		});
	}

	glm::mat4 TerrainEditor::GetTerrainScaleAsMatrix() const
	{
		return glm::scale(glm::mat4(1.0f), mTerrainScale);
	}

	glm::vec3 TerrainEditor::HeightMapToWorldCoord(glm::ivec2 hmCoord) const
	{
		glm::vec4 originalSpace{ hmCoord.x, GetHeightMap().GetElevation(hmCoord.x, hmCoord.y), hmCoord.y, 1.0f }; 
		return GetTerrainScaleAsMatrix() * originalSpace;
	}
}

