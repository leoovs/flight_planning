#pragma once

#include <cinttypes>

#include <utility>

#include "platform/mouse_button.h"

namespace editor
{
	class Mouse
	{
	public:
		virtual ~Mouse() = default;

		virtual std::pair<int32_t, int32_t> GetCursorCoords() const = 0;

		virtual bool IsButtonPressed(MouseButton button) const = 0;
	};
}
