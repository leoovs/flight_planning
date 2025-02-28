#pragma once

#include <cinttypes>

#include "event/event.h"

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
}

