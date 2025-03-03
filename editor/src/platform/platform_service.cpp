#include "platform/platform_service.h"

#include <uavpf/uavpf.h>

#include "platform_sdl3/sdl3_platform_service.h"

namespace editor
{
	PlatformService* CreatePlatformService()
	{
		return new Sdl3PlatformService();
	}

	void DestroyPlatformService(PlatformService* platform)
	{
		if (nullptr == platform)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy platform service which is nullptr");
		}

		delete platform;
	}
}
