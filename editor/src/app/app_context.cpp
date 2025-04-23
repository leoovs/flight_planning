#include "app/app_context.h"

namespace editor
{
	AppContext::AppContext(AppService* service)
		: mService(service)
	{
	}

	AssetStorage& AppContext::GetAssets()
	{
		return mAssets;
	}
}

