#pragma once

#include <cinttypes>

#include "platform/mouse.h"

namespace editor
{
	class Sdl3Mouse final : public Mouse
	{
	public:
		static MouseButton ConvertNativeButtonToButton(uint32_t nativeButton);
		static uint32_t ConvertButtonToNativeMask(MouseButton button);

		~Sdl3Mouse() override = default;

		std::pair<int32_t, int32_t> GetCursorCoords() const override;

		bool IsButtonPressed(MouseButton button) const override;
	};
}
