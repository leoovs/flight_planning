#include "platform_sdl3/sdl3_platform_service.h"

#include <SDL3/SDL_events.h>
#include <uavpf/uavpf.h>

#include "event/event_bus.h"
#include "platform/platform_events.h"

namespace editor
{
	Sdl3PlatformService::Sdl3PlatformService()
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not initialize SDL3: %s",
				SDL_GetError());
		}

		SetupNativeEventHandlers();
	}

	Sdl3PlatformService::~Sdl3PlatformService()
	{
		SDL_Quit();
	}

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
			uint32_t nativeEventType = event.type;

			if (mHandlersByNativeEventType.count(nativeEventType) != 0)
			{
				std::invoke(
					mHandlersByNativeEventType.at(nativeEventType),
					this,
					event);
			}
		}
	}

	Sdl3Window* Sdl3PlatformService::CreateWindow(const WindowParams& params)
	{
		Sdl3Window* window = new Sdl3Window(params);
		RegisterWindow(window);

		return window;
	}

	void Sdl3PlatformService::DestroyWindow(Window* window)
	{
		if (nullptr == window)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy window which is nullptr");
		}
		UnregisterWindow(dynamic_cast<Sdl3Window*>(window));
		delete window;
	}

	Sdl3Mouse* Sdl3PlatformService::CreateMouse()
	{
		if (nullptr != mRegisteredMouse)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to create second mouse");
			return mRegisteredMouse;
		}
		return mRegisteredMouse = new Sdl3Mouse();
	}

	void Sdl3PlatformService::DestroyMouse(Mouse* mouse)
	{
		if (nullptr == mouse)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy mouse which is nullptr");
		}
		delete mouse;
		mRegisteredMouse = nullptr;
	}

	void Sdl3PlatformService::RegisterWindow(Sdl3Window* window)
	{
		if (nullptr == window)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to register nullptr window");
			return;
		}

		SDL_Window* nativeWindow = window->GetNativeWindow();
		SDL_WindowID nativeWindowID = SDL_GetWindowID(nativeWindow); 

		auto entry = std::make_pair(nativeWindowID, window);
		mWindowsByNativeWindowID.insert(entry);
	}

	void Sdl3PlatformService::UnregisterWindow(Sdl3Window* window)
	{
		if (nullptr == window)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to unregister nullptr window");
			return;
		}

		SDL_Window* nativeWindow = window->GetNativeWindow();
		SDL_WindowID nativeWindowID = SDL_GetWindowID(nativeWindow); 

		if (mWindowsByNativeWindowID.count(nativeWindowID) == 0)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to unregister window that is seemingly valid,"
				" but unregistered");
			return;
		}

		mWindowsByNativeWindowID.erase(nativeWindowID);
	}

	Sdl3Window* Sdl3PlatformService::FindRegisteredWindowByNativeWindowID(
		SDL_WindowID id) const
	{
		if (mWindowsByNativeWindowID.count(id) == 0)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to map native window ID to unregistered window");
			return nullptr;
		}

		return mWindowsByNativeWindowID.at(id);
	}

	void Sdl3PlatformService::SetupNativeEventHandlers()
	{
		mHandlersByNativeEventType =
		{
			{ SDL_EVENT_WINDOW_RESIZED, &Sdl3PlatformService::Native_OnWindowResize },
			{ SDL_EVENT_WINDOW_CLOSE_REQUESTED, &Sdl3PlatformService::Native_OnWindowClose },
			{ SDL_EVENT_MOUSE_BUTTON_DOWN, &Sdl3PlatformService::Native_OnMouseButtonDown },
			{ SDL_EVENT_MOUSE_BUTTON_UP, &Sdl3PlatformService::Native_OnMouseButtonUp },
		};
	}

	void Sdl3PlatformService::Native_OnWindowResize(const SDL_Event& nativeEvent)
	{
		auto nativeWindowID = nativeEvent.window.windowID;
		auto width = static_cast<int32_t>(nativeEvent.window.data1);
		auto height = static_cast<int32_t>(nativeEvent.window.data2);

		Sdl3Window* resizedWindow = FindRegisteredWindowByNativeWindowID(
			nativeWindowID);

		mPlatformEventPublisher.Publish<WindowResizeEvent>(
			EventPublishMode::Queued,
			width,
			height,
			resizedWindow);
	}

	void Sdl3PlatformService::Native_OnWindowClose(const SDL_Event& nativeEvent)
	{
		auto nativeWindowID = nativeEvent.window.windowID;

		Sdl3Window* closedWindow = FindRegisteredWindowByNativeWindowID(
			nativeWindowID);

		mPlatformEventPublisher.Publish<WindowCloseEvent>(
			EventPublishMode::Queued,
			closedWindow);
	}

	void Sdl3PlatformService::Native_OnMouseButtonDown(const SDL_Event& nativeEvent)
	{
		MouseButton buttonDown = mRegisteredMouse->ConvertNativeButtonToButton(nativeEvent.button.button);
		
		mPlatformEventPublisher.Publish<MouseButtonDownEvent>(
			EventPublishMode::Queued,
			mRegisteredMouse,
			buttonDown);
	}

	void Sdl3PlatformService::Native_OnMouseButtonUp(const SDL_Event& nativeEvent)
	{
		MouseButton buttonUp = mRegisteredMouse->ConvertNativeButtonToButton(nativeEvent.button.button);

		mPlatformEventPublisher.Publish<MouseButtonUpEvent>(
			EventPublishMode::Queued,
			mRegisteredMouse, 
			buttonUp);
	}
}
