#include "platform_sdl3/sdl3_platform_service.h"

#include <SDL3/SDL_events.h>

#include "event/event_bus.h"

namespace editor
{
	std::string_view Sdl3PlatformService::GetName() const
	{
		return "SDL3";
	}
	void Sdl3PlatformService::BeginFrame()
	{
	}
	void Sdl3PlatformService::EndFrame()
	{
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
