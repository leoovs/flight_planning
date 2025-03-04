#pragma once

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "graphics_ogl/ogl_provider.h"
#include "platform_sdl3/sdl3_window.h"

namespace editor
{
	struct Sdl3OglProviderParams
	{
		Sdl3Window* OutputWindow = nullptr;
		bool IsContextDebuggable = false;
	};

	class Sdl3OglProvider final : public OglProvider
	{
	public:
		Sdl3OglProvider(Sdl3OglProviderParams params);
		~Sdl3OglProvider() override;

		void SwapBuffers() override;

		const Sdl3OglProviderParams& GetParams() const;

	private:
		void AcquireNativeContext();
		void ReleaseNativeContext();

		Sdl3OglProviderParams mParams;
		SDL_Window* mNativeWindow = nullptr;
		SDL_GLContext mNativeContext = nullptr;
	};
}

