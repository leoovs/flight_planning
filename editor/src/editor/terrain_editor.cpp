#include "editor/terrain_editor.h"

#include <glm/ext/matrix_transform.hpp>

#include "asset/height_map_asset.h"

#include "runtime/rt_module_locator.h"

namespace editor
{
	void TerrainEditor::LoadHeightMap(const std::filesystem::path& path)
	{
		mHeightMap = mAssets->Load(path, AssetKind::HeightMap);
		if (uavpf::cBadID == mHeightMap)
		{
			return;
		}

		auto hm = mAssets->Get<HeightMapAsset>(mHeightMap)->GetHeightMap();
		mHeightMapResolution = {
			hm.GetWidth(),
			hm.GetDepth(),
		};

		ScaleToFitUnitSquare();
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

	glm::mat4 TerrainEditor::GetHeightMapToWorldMatrix() const
	{
		return glm::scale(glm::mat4(1.0f), mHeightMapToTerrainScale*mWorldScale);
	}

	void TerrainEditor::SetWorldScale(float scale)
	{
		mWorldScale = scale;
	}

	float TerrainEditor::GetWorldScale() const
	{
		return mWorldScale;
	}

	void TerrainEditor::SetHeightScale(float scale)
	{
		mHeightMapToTerrainScale.y = scale;
	}

	float TerrainEditor::GetHeightScale() const
	{
		return mHeightMapToTerrainScale.y;
	}

	void TerrainEditor::ScaleToFitUnitSquare()
	{
		mHeightMapToTerrainScale = {
			1.0f / mHeightMapResolution.x,
			1.0f,
			1.0f / mHeightMapResolution.y
		};
	}

	glm::vec3 TerrainEditor::HeightMapToWorldCoord(glm::ivec2 hmCoord) const
	{
		glm::vec4 originalSpace{ hmCoord.x, GetHeightMap().GetElevation(hmCoord.x, hmCoord.y), hmCoord.y, 1.0f }; 
		return GetHeightMapToWorldMatrix() * originalSpace;
	}
}

