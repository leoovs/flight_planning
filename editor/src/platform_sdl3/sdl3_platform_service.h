#pragma once

#include <unordered_map>

#include "event/event_publisher.h"
#include "platform/platform_service.h"
#include "platform_sdl3/sdl3_keyboard.h"
#include "platform_sdl3/sdl3_mouse.h"
#include "platform_sdl3/sdl3_window.h"

namespace editor
{
	class OglGraphicsContext;

	class Sdl3PlatformService final : public PlatformService
	{
	public:
		Sdl3PlatformService();
		~Sdl3PlatformService() override;

		std::string_view GetName() const override;

		void BeginFrame() override;
		void EndFrame() override;
		 
		void BindEvents(EventBus& events) override;
		void PollEvents() override;

		Sdl3Window* CreateWindow(const WindowParams& params) override;
		void DestroyWindow(Window* window) override;

		Sdl3Mouse* CreateMouse() override;
		void DestroyMouse(Mouse* mouse) override;

		Sdl3Keyboard* CreateKeyboard() override;
		void DestroyKeyboard(Keyboard* keyboard) override;

		GraphicsContext* CreateGraphicsContext(GraphicsContextParams params) override;
		void DestroyGraphicsContext(GraphicsContext* graphicsContext) override;

	private:
		void RegisterWindow(Sdl3Window* window);
		void UnregisterWindow(Sdl3Window* window);
		Sdl3Window* FindRegisteredWindowByNativeWindowID(SDL_WindowID id) const;

		void SetupNativeEventHandlers();

		OglGraphicsContext* CreateOglGraphicsContext(GraphicsContextParams params);

		void Native_OnWindowResize(const SDL_Event& nativeEvent);
		void Native_OnWindowClose(const SDL_Event& nativeEvent);
		void Native_OnMouseButtonDown(const SDL_Event& nativeEvent);
		void Native_OnMouseButtonUp(const SDL_Event& nativeEvent);
		void Native_OnMouseMovement(const SDL_Event& nativeEvent);
		void Native_OnKeyDown(const SDL_Event& nativeEvent);
		void Native_OnKeyUp(const SDL_Event& nativeEvent);

		using NativeEventHandler = void(Sdl3PlatformService::*)(const SDL_Event&);

		EventPublisher mPlatformEventPublisher;
		std::unordered_map<SDL_WindowID, Sdl3Window*> mWindowsByNativeWindowID;
		std::unordered_map<uint32_t, NativeEventHandler> mHandlersByNativeEventType;
		Sdl3Mouse* mRegisteredMouse = nullptr;
		Sdl3Keyboard* mRegisteredKeyboard = nullptr;
		GraphicsContext* mRegisteredGraphicsContext = nullptr;
	};
}
