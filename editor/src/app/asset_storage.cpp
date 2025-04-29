#include "app/asset_storage.h"

#include "app/app_service.h"

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// ImageAsset
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	AssetKind ImageAsset::GetKind() const
	{
		return AssetKind::Image;
	}

	void ImageAsset::SetImage(uavpf::TiffImage image)
	{
		mImage = std::move(image);
	}

	uavpf::TiffImage& ImageAsset::GetImage()
	{
		return mImage;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// HeightMapAsset 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	AssetKind HeightMapAsset::GetKind() const
	{
		return AssetKind::HeightMap;
	}

	void HeightMapAsset::SetHeightMap(uavpf::HeightMap heightMap)
	{
		mHeightMap = std::move(heightMap);
	}

	uavpf::HeightMap& HeightMapAsset::GetHeightMap()
	{
		return mHeightMap;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// TerrainAsset 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	AssetKind TerrainAsset::GetKind() const
	{
		return AssetKind::Terrain;
	}

	void TerrainAsset::SetMesh(std::unique_ptr<TerrainRenderMesh> mesh)
	{
		mMesh = std::move(mesh);
	}

	TerrainRenderMesh* TerrainAsset::GetMesh()
	{
		return mMesh.get();
	}
		
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// TextureAsset 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	AssetKind TextureAsset::GetKind() const
	{
		return AssetKind::Texture;
	}


	void TextureAsset::SetTexture(std::shared_ptr<Texture2D> texture)
	{
		mTexture = std::move(texture);
	}

	Texture2D* TextureAsset::GetTexture() const
	{
		return mTexture.get();
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// AssetStorage 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	AssetStorage::AssetStorage(AppService* service)
		: mService(service)
	{
	}

	AssetID AssetStorage::LoadAsset(const std::filesystem::path& assetPath, AssetKind kind)
	{
		if (mAssetIDByPath.count(assetPath))
		{
			return mAssetIDByPath.at(assetPath);
		}

		AssetID id = uavpf::cBadID;
		switch (kind)
		{
			case AssetKind::Unknown:
				return uavpf::cBadID;
			case AssetKind::Image:
				id = LoadAsset_Image(assetPath);
			default:
				break;
		}

		if (uavpf::cBadID == id)
		{
			return id;
		}

		mAssetIDByPath.insert(std::make_pair(assetPath, id));
		if (mAssetPathByID.size() <= id)
		{
			mAssetPathByID.resize(id + 1);
		}
		mAssetPathByID.at(id) = assetPath;

		return id;
	}

	void AssetStorage::LoadAssetAsync(
		const std::filesystem::path& assetPath,
		AssetKind kind,
		std::function<void(AssetID)> onComplete)
	{
		mService->AddTask(
			[this, assetPath, kind, onComplete]()
			{
				AssetID id = LoadAsset(assetPath, kind);
				onComplete(id);
			}
		);
	}

	AssetID AssetStorage::CreateAsset(AssetKind kind)
	{
		if (AssetKind::Unknown == kind)
		{
			return uavpf::cBadID;
		}

		AssetID id = mAssetIDs.Allocate();
		switch (kind)
		{
		case AssetKind::Image:
			Save(id, std::make_unique<ImageAsset>());
			break;
		case AssetKind::HeightMap:
			Save(id, std::make_unique<HeightMapAsset>());
			break;
		case AssetKind::Terrain:
			Save(id, std::make_unique<TerrainAsset>());
			break;
		case AssetKind::Texture:
			Save(id, std::make_unique<TextureAsset>());
			break;
		default:
			break;
		}

		return id;
	}

	void AssetStorage::UnloadAsset(AssetID id)
	{
		if (!mAssetIDs.IsActive(id))
		{
			return;
		}

		mAssetIDs.Free(id);
		mAssetByID.at(id).reset();

		const std::filesystem::path& assetPath = mAssetPathByID.at(id);

		mAssetIDByPath.erase(assetPath);
		mAssetPathByID.at(id).clear();
	}

	bool AssetStorage::Exists(AssetID id) const
	{
		return mAssetIDs.IsActive(id);
	}

	bool AssetStorage::IsOfKind(AssetID id, AssetKind kind) const
	{
		return Exists(id)
			&& mAssetByID.at(id)->GetKind() == kind;
	}

	Asset* AssetStorage::GetAssetFromPath(const std::filesystem::path& assetPath) const
	{
		if (mAssetIDByPath.count(assetPath))
		{
			AssetID id = mAssetIDByPath.at(assetPath);
			return GetAssetFromID(id);
		}

		return nullptr;
	}

	Asset* AssetStorage::GetAssetFromID(AssetID id) const
	{
		return mAssetIDs.IsActive(id)
			? mAssetByID.at(id).get()
			: nullptr;
	}

	void AssetStorage::Save(AssetID id, std::unique_ptr<Asset> asset)
	{
		if (id >= mAssetByID.size())
		{
			mAssetByID.resize(id + 1);
		}

		mAssetByID.at(id) = std::move(asset);
	}

	AssetID AssetStorage::LoadAsset_Image(const std::filesystem::path& imagePath)
	{
		if (!std::filesystem::exists(imagePath))
		{
			return uavpf::cBadID;
		}

		uavpf::TiffImage image = uavpf::TiffLoader()
			.LoadImageFromFile(imagePath.string());

		if (!image)
		{
			return uavpf::cBadID;
		}

		AssetID id = CreateAsset(AssetKind::Image);
		GetAssetFromID<ImageAsset>(id)->SetImage(std::move(image));

		return id;
	}
}

