#include "platform_sdl3/sdl3_platform_service.h"

#include <SDL3/SDL_events.h>
#include <uavpf/uavpf.h>

#include "event/event_bus.h"
#include "platform/platform_events.h"

namespace editor
{
	std::string_view Sdl3PlatformService::GetName() const
	{
		return "SDL3";
	}

	void Sdl3PlatformService::BeginFrame()
	{
		mPlatformEventPublisher.Publish<BeginFrameEvent>(EventPublishMode::Immediate);
	}

	void Sdl3PlatformService::EndFrame()
	{
		mPlatformEventPublisher.Publish<EndFrameEvent>(EventPublishMode::Immediate);
	}

	void Sdl3PlatformService::BindEvents(EventBus& events)
	{
		mPlatformEventPublisher = EventPublisher(events);
	}

	void Sdl3PlatformService::PollEvents()
	{
		for (SDL_Event event; SDL_PollEvent(&event);)
		{
			switch (event.type)
			{
			case SDL_EVENT_WINDOW_RESIZED:
				//mEventBus.Publish<WindowResizedEvent>(event.window.data1, event.window.data2);
				break;
			}
		}
	}

	Sdl3Window* Sdl3PlatformService::CreateWindow(const WindowParams& params)
	{
		return new Sdl3Window(params);
	}

	void Sdl3PlatformService::DestroyWindow(Window* window)
	{
		delete window;
	}
}
