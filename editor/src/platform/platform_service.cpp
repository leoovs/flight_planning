#include "platform/platform_service.h"
#include "platform_sdl3/sdl3_platform_service.h"

namespace editor
{
	PlatformService* CreatePlatformService()
	{
		PlatformService* platform = new Sdl3PlatformService();

		return platform;
	}

	void DestroyPlatformService(PlatformService* platform)
	{
		delete platform;
	}
}
