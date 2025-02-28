#include "platform_sdl3/sdl3_window.h"

namespace editor
{
	std::string_view Sdl3Window::SetWindowName() const
	{
		return "SDL3";
	}
	int Sdl3Window::ScreenWidth()
	{
		return 800;
	}
	int Sdl3Window::ScreenHeight()
	{
		return 800;
	}
	void Sdl3Window::PollEvents()
	{
	}
}
