#pragma once

#include <array>
#include <memory>
#include <unordered_map>

#include <uavpf/id/id_allocator.h>

#include "asset/asset_importer.h"

namespace editor
{
	using AssetID = uavpf::BaseID;

	class AssetStorage
	{
	public:
		template<typename AssetImporterT>
		void RegisterImporter()
		{
			auto importer = std::make_unique<AssetImporterT>();
			mImporters.at(+importer->GetImportedAssetKind()) = std::move(importer);
		}

		AssetID Load(std::filesystem::path assetPath, AssetKind kind);
		void Unload(AssetID id);

		template<typename AssetT>
		std::shared_ptr<AssetT> Get(AssetID id)
		{
			return std::dynamic_pointer_cast<AssetT>(mAssetByID.at(id));
		}

	private:
		AssetImporter* GetImporter(AssetKind kind) const;

		AssetID RegisterAsset(std::filesystem::path assetPath, std::shared_ptr<Asset> asset);

		std::array<std::unique_ptr<AssetImporter>, size_t(AssetKind::Count_)> mImporters;	
		uavpf::IDAllocator mAssetIDRegistry;
		std::unordered_map<AssetID, std::shared_ptr<Asset>> mAssetByID;
		std::unordered_map<std::filesystem::path, AssetID> mAssetIDByPath;
		std::unordered_map<AssetID, std::filesystem::path> mAssetPathByID;
	};
}

