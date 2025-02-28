#pragma once

#include "platform/window.h"

namespace editor
{
	class Sdl3Window final : public Window
	{
	public:
		~Sdl3Window() override = default;

		std::string_view SetWindowName() const override;

		int ScreenWidth() override;
		int ScreenHeight() override;

		void PollEvents() override;
	};
}
