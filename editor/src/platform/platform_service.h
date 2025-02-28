#pragma once

#include <string_view>

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
	};

	// Фабричные функции для инициализации и разрушения.
	PlatformService* CreatePlatformService();
	void DestroyPlatformService(PlatformService* platform);
}
