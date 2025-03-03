#pragma once

#include <cinttypes>

#include <string>
#include <string_view>

namespace editor
{
	struct WindowParams
	{
		std::string Title = "flight_planning";
		int32_t Width = 1280;
		int32_t Height = 720;
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual const WindowParams& GetParams() const = 0;

		std::string_view GetTitle() const
		{
			return GetParams().Title;
		}

		int32_t GetWidth() const
		{
			return GetParams().Width;
		}

		int32_t GetHeight() const
		{
			return GetParams().Height;
		}

		virtual void SetTitle(std::string_view title) = 0;
		virtual void SetWidth(int32_t width) = 0;
		virtual void SetHeight(int32_t height) = 0;
	};
}
