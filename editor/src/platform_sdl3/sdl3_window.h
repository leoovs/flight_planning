#pragma once

#include <SDL3/SDL.h>

#include "platform/window.h"

namespace editor
{
	class Sdl3Window final : public Window
	{
	public:
		Sdl3Window(const WindowParams& params);
		~Sdl3Window() override;

		const WindowParams& GetParams() const override;

		void SetTitle(std::string_view title) override;
		void SetWidth(int32_t width) override;
		void SetHeight(int32_t height) override;

		SDL_Window* GetNativeWindow() const;

	private:
		void CreateNativeWindow();
		void DestroyNativeWindow();

		WindowParams mParams;
		SDL_Window* mNativeWindow = nullptr;
	};
}
