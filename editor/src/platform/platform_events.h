#pragma once

#include <cinttypes>

#include "event/event.h"
#include "platform/key.h"
#include "platform/mouse_button.h"

namespace editor
{
	class BeginFrameEvent : public Event
	{
	};

	class EndFrameEvent : public Event
	{
	};

	class Window;

	class WindowResizeEvent : public Event
	{
	public:
		int32_t Width = 0;
		int32_t Height = 0;
		Window* ResizedWindow = nullptr;

		WindowResizeEvent(int32_t width, int32_t height, Window* resizedWindow)
			: Width(width)
			, Height(height)
			, ResizedWindow(resizedWindow)
		{
		}
	};

	class WindowCloseEvent : public Event
	{
	public:
		Window* ClosedWindow = nullptr;

		WindowCloseEvent(Window* closedWindow)
			: ClosedWindow(closedWindow)
		{
		}
	};

	class Keyboard;

	class KeyDownEvent : public Event
	{
	public:
		Keyboard* RegisteredKeyboard = nullptr;
		Key KeyDown = Key::Unknown;

		KeyDownEvent(Keyboard* registeredKeyboard, Key keyDown)
			: RegisteredKeyboard(registeredKeyboard)
			, KeyDown(keyDown)
		{
		}
	};

	class KeyUpEvent : public Event
	{
	public:
		Keyboard* RegisteredKeyboard = nullptr;
		Key KeyUp = Key::Unknown;

		KeyUpEvent(Keyboard* registeredKeyboard, Key keyUp)
			: RegisteredKeyboard(registeredKeyboard)
			, KeyUp(keyUp)
		{
		}
	};

	class Mouse;

	class MouseButtonDownEvent : public Event
	{
	public:
		Mouse* RegisteredMouse = nullptr;
		MouseButton ButtonDown = MouseButton::Unknown;

		MouseButtonDownEvent(Mouse* registeredMouse, MouseButton buttonDown)
			: RegisteredMouse(registeredMouse)
			, ButtonDown(buttonDown)
		{
		}
	};

	class MouseButtonUpEvent : public Event
	{
	public:
		Mouse* RegisteredMouse = nullptr;
		MouseButton ButtonUp = MouseButton::Unknown;

		MouseButtonUpEvent(Mouse* registeredMouse, MouseButton buttonUp)
			: RegisteredMouse(registeredMouse)
			, ButtonUp(buttonUp)
		{
		}
	};

	class MouseMovementEvent : public Event
	{
	public:
		Mouse* RegisteredMouse = nullptr;
		int32_t DeltaX = 0;
		int32_t DeltaY = 0;

		MouseMovementEvent(
			Mouse* registeredMouse,
			int32_t deltaX,
			int32_t deltaY)
			: RegisteredMouse(registeredMouse)
			, DeltaX(deltaX)
			, DeltaY(deltaY)
		{
		}
	};
}

