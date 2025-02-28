#pragma once

#include <string_view>

#include "platform/window.h"

namespace editor
{
	class PlatformService
	{
	public:
		virtual ~PlatformService() = default;

		virtual std::string_view GetName() const = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void PollEvents() = 0;

		virtual Window* CreateWindow(const WindowParams& params = {}) = 0;
		virtual void DestroyWindow(Window* window) = 0;
	};

	PlatformService* CreatePlatformService();
	void DestroyPlatformService(PlatformService* platform);
}
