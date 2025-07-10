#include "asset/asset_storage.h"

namespace editor
{
	AssetID AssetStorage::Load(std::filesystem::path assetPath, AssetKind kind)
	{
		if (mAssetIDByPath.count(assetPath))
		{
			return mAssetIDByPath.at(assetPath);
		}

		AssetImporter* importer = GetImporter(kind);
		if (nullptr == importer)
		{
			return uavpf::cBadID;
		}

		std::shared_ptr<Asset> asset = importer->Import(assetPath);
		if (nullptr == asset)
		{
			return uavpf::cBadID;
		}

		return RegisterAsset(assetPath, asset);
	}

	void AssetStorage::Unload(AssetID id)
	{
		mAssetByID.erase(id);
		mAssetIDRegistry.Free(id);
		mAssetIDByPath.erase(mAssetPathByID.at(id));
		mAssetPathByID.erase(id);
	}

	AssetImporter* AssetStorage::GetImporter(AssetKind kind) const
	{
		return mImporters.at(+kind).get();
	}

	AssetID AssetStorage::RegisterAsset(
		std::filesystem::path assetPath,
		std::shared_ptr<Asset> asset)
	{
		AssetID id = mAssetIDRegistry.Allocate();
		mAssetByID.insert(std::make_pair(id, asset));
		mAssetIDByPath.insert(std::make_pair(assetPath, id));
		mAssetPathByID.insert(std::make_pair(id, assetPath));

		return id;
	}
}
