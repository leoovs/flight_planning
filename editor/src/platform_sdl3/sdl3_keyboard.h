#pragma once

#include <SDL3/SDL.h>

#include "platform/keyboard.h"

namespace editor
{
	class Sdl3Keyboard final : public Keyboard
	{
	public:
		static SDL_Keycode ConvertKeyToNativeKey(Key key);
		static Key ConvertNativeKeyToKey(SDL_Keycode nativeKey);

		bool IsKeyDown(Key key) const override;

	private:
	};
}

