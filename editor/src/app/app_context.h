#pragma once

#include "app/asset_storage.h"
#include "app/app_service.h"
#include "app/path_mission.h"

namespace editor
{
	class AppContext
	{
	public:
		AppContext(AppService* service);

		AssetStorage& GetAssets();

		void SetMapImage(AssetID mapImage);
		uavpf::TiffImage* GetMapImage();

		PathMission& GetPathMission();

	private:
		AppService* mService = nullptr;
		AssetStorage mAssets;	

		AssetID mMapImage = uavpf::cBadID;
		PathMission mPathMission;
	};
}

