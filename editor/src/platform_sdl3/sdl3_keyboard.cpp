#include "platform_sdl3/sdl3_keyboard.h"

#include <cassert>

namespace editor
{
	SDL_Keycode Sdl3Keyboard::ConvertKeyToNativeKey(Key key)
	{
		switch (key)
		{
			case Key::Unknown:
				return SDLK_UNKNOWN;
			case Key::W:
				return SDLK_W;
			case Key::A:
				return SDLK_A;
			case Key::S:
				return SDLK_S;
			case Key::D:
				return SDLK_D;

			default:
				assert(false && "Invalid Key enum value");
				return SDLK_UNKNOWN;
		}
	}

	Key Sdl3Keyboard::ConvertNativeKeyToKey(SDL_Keycode nativeKey)
	{
		switch (nativeKey)
		{
			case SDLK_UNKNOWN:
				return Key::Unknown;
			case SDLK_W:
				return Key::W;
			case SDLK_A:
				return Key::A;
			case SDLK_S:
				return Key::S;
			case SDLK_D:
				return Key::D;
		}

		return Key::Unknown;
	}

	bool Sdl3Keyboard::IsKeyDown(Key key) const
	{
		const bool* isPressed = SDL_GetKeyboardState(nullptr);
		SDL_Keycode nativeKey = ConvertKeyToNativeKey(key);
		SDL_Scancode scancode = SDL_GetScancodeFromKey(nativeKey, nullptr);

		return isPressed[scancode];
	}
}

