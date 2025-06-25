#pragma once

#include <unordered_map>
#include <filesystem>
#include <future>

#include <uavpf/uavpf.h>

#include "rendering/terrain_render_mesh.h"

namespace editor
{
	class AppService;

	using AssetID = uavpf::BaseID;

	enum class AssetKind
	{
		Unknown,
		Image,
		HeightMap,
		Terrain,
		Texture,
	};

	class Asset
	{
	public:
		virtual ~Asset() = default;

		virtual AssetKind GetKind() const = 0;
	};

	class ImageAsset final : public Asset
	{
	public:
		~ImageAsset() override = default;

		AssetKind GetKind() const override;

		void SetImage(uavpf::TiffImage image);
		uavpf::TiffImage& GetImage();

	private:
		uavpf::TiffImage mImage;
	};

	class HeightMapAsset final : public Asset 
	{
	public:
		~HeightMapAsset() override = default;

		AssetKind GetKind() const override;

		void SetHeightMap(uavpf::HeightMap heightMap);
		uavpf::HeightMap& GetHeightMap();
		
	private:
		uavpf::HeightMap mHeightMap;
	};

	class TerrainAsset final : public Asset 
	{
	public:
		~TerrainAsset() override = default;

		AssetKind GetKind() const override;

		void SetMesh(std::unique_ptr<TerrainRenderMesh> mesh);
		TerrainRenderMesh* GetMesh();
		
	private:
		std::unique_ptr<TerrainRenderMesh> mMesh;
	};

	class TextureAsset final : public Asset
	{
	public:
		~TextureAsset() override = default;

		AssetKind GetKind() const override;

		void SetTexture(std::shared_ptr<Texture2D> texture);
		Texture2D* GetTexture() const;

	private:
		std::shared_ptr<Texture2D> mTexture;
	};

	class AssetStorage
	{
	public:
		AssetStorage(AppService* service);

		AssetID LoadAsset(const std::filesystem::path& assetPath, AssetKind kind);

		AssetID CreateAsset(AssetKind kind);
		void UnloadAsset(AssetID id);

		bool Exists(AssetID id) const;
		bool IsOfKind(AssetID id, AssetKind kind) const;
		Asset* GetAssetFromPath(const std::filesystem::path& assetPath) const;
		Asset* GetAssetFromID(AssetID id) const;

		template<typename AssetT, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetT>>>
		AssetT* GetAssetFromPath(const std::filesystem::path& assetPath) const
		{
			return dynamic_cast<AssetT*>(GetAssetFromPath(assetPath));
		}

		template<typename AssetT, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetT>>>
		AssetT* GetAssetFromID(AssetID id) const
		{
			return dynamic_cast<AssetT*>(GetAssetFromID(id));
		}

	private:
		void Save(AssetID id, std::unique_ptr<Asset> asset);

		AssetID LoadAsset_Image(const std::filesystem::path& imagePath);

		AppService* mService = nullptr;

		uavpf::IDAllocator mAssetIDs;
		std::unordered_map<std::string, AssetID> mAssetIDByPath;
		std::vector<std::unique_ptr<Asset>> mAssetByID;
		std::vector<std::filesystem::path> mAssetPathByID;
	};
}

