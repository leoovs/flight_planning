#include "platform_sdl3/sdl3_mouse.h"

#include <cassert>

#include <SDL3/SDL.h>

namespace editor 
{
	MouseButton Sdl3Mouse::ConvertNativeButtonToButton(uint32_t nativeButton)
	{
		switch (nativeButton)
		{
			case SDL_BUTTON_LEFT:
				return MouseButton::Left;
			case SDL_BUTTON_RIGHT:
				return MouseButton::Right;
			case SDL_BUTTON_MIDDLE:
				return MouseButton::Wheel;
			default:
				return MouseButton::Unknown;
		}
	}

	uint32_t Sdl3Mouse::ConvertButtonToNativeMask(MouseButton button)
	{
		switch (button)
		{
			case MouseButton::Unknown:
				return 0u;
			case MouseButton::Left:
				return SDL_BUTTON_LMASK;
			case MouseButton::Right:
				return SDL_BUTTON_RMASK;
			case MouseButton::Wheel:
				return SDL_BUTTON_MMASK;
			default:
				assert(false && "Invalid MouseButton enum value");
		}
	}

	std::pair<int32_t, int32_t> Sdl3Mouse::GetCursorCoords() const
	{
		float coordX = 0.0f;
		float coordY = 0.0f;
		SDL_GetMouseState(&coordX, &coordY);

		return std::make_pair(
			static_cast<int32_t>(coordX),
			static_cast<int32_t>(coordY));
	}

	bool Sdl3Mouse::IsButtonPressed(MouseButton button) const
	{
		uint32_t mask = ConvertButtonToNativeMask(button);
		if (SDL_GetMouseState(nullptr, nullptr) & mask)
		{
			return true;
		}
		return false;
	}
}
