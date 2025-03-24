#pragma once

#include "platform/key.h"

namespace editor
{
	class Keyboard
	{
	public:
		virtual ~Keyboard() = default;

		virtual bool IsKeyDown(Key key) const = 0;
	};
}

