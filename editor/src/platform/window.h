#pragma once

#include <string_view>

namespace editor
{
	class Window
	{
	public:
		virtual ~Window() = default;

		virtual std::string_view SetWindowName() const = 0;

		virtual int ScreenWidth() = 0;
		virtual int ScreenHeight() = 0;

		virtual void PollEvents() = 0;
	};
}
