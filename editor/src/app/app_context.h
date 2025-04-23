#pragma once

#include "app/asset_storage.h"
#include "app/app_service.h"

namespace editor
{
	enum class AppState
	{
		Operating,
		PathPlanning,
	};

	class AppContext
	{
	public:
		AppContext(AppService* service);

		AssetStorage& GetAssets();

	private:
		AppService* mService = nullptr;
		AssetStorage mAssets;	
	};
}

