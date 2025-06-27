#pragma once

#include <memory>
#include <filesystem>

#include "asset/asset.h"

namespace editor
{
	class AssetImporter
	{
	public:
		virtual ~AssetImporter() = default;

		virtual AssetKind GetImportedAssetKind() const = 0;
		virtual std::shared_ptr<Asset> Import(std::filesystem::path assetPath) = 0;	
		virtual bool IsSuccessful() const = 0;
	};
}

