#include "app/app_context.h"

namespace editor
{
	AppContext::AppContext(AppService* service)
		: mService(service)
		, mAssets(service)
	{
	}

	AssetStorage& AppContext::GetAssets()
	{
		return mAssets;
	}

	void AppContext::SetMapImage(AssetID mapImage)
	{
		if (mAssets.IsOfKind(mapImage, AssetKind::Image))
		{
			mMapImage = mapImage;
		}
	}

	uavpf::TiffImage* AppContext::GetMapImage()
	{
		if (uavpf::cBadID == mMapImage)
		{
			return nullptr;
		}

		return &mAssets
			.GetAssetFromID<ImageAsset>(mMapImage)
			->GetImage();
	}

	PathMission& AppContext::GetPathMission()
	{
		return mPathMission;
	}
}

