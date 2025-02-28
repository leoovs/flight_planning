#pragma once

#include "event/event_publisher.h"
#include "platform/platform_service.h"
#include "platform_sdl3/sdl3_window.h"

namespace editor
{
	class Sdl3PlatformService final : public PlatformService
	{
	public:
		~Sdl3PlatformService() override = default;

		std::string_view GetName() const override;

		void BeginFrame() override;
		void EndFrame() override;
		 
		void BindEvents(EventBus& events) override;
		void PollEvents() override;

		Sdl3Window* CreateWindow(const WindowParams& params) override;
		void DestroyWindow(Window* window) override;

	private:
		EventPublisher mPlatformEventPublisher;
	};
}
