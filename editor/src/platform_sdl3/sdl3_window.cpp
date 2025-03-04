#include "platform_sdl3/sdl3_window.h"

#include <uavpf/uavpf.h>

namespace editor
{
	Sdl3Window::Sdl3Window(const WindowParams& params)
		: mParams(params)
	{
		CreateNativeWindow();
	}

	Sdl3Window::~Sdl3Window()
	{
		DestroyNativeWindow();
	}

	const WindowParams& Sdl3Window::GetParams() const
	{
		return mParams;
	}

	void Sdl3Window::SetTitle(std::string_view title)
	{
		bool success = SDL_SetWindowTitle(
			mNativeWindow,
			(mParams.Title = title).data());

		if (!success)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not set SDL3 window title: %s",
				SDL_GetError());
		}
	}

	void Sdl3Window::SetWidth(int32_t width)
	{
		bool success = SDL_SetWindowSize(
			mNativeWindow,
			mParams.Width = width,
			mParams.Height);

		if (!success)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not set SDL3 window width: %s",
				SDL_GetError());
		}
	}

	void Sdl3Window::SetHeight(int32_t height)
	{
		bool success = SDL_SetWindowSize(
			mNativeWindow,
			mParams.Width,
			mParams.Height = height);

		if (!success)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not set SDL3 window height: %s",
				SDL_GetError());
		}
	}

	SDL_Window* Sdl3Window::GetNativeWindow() const
	{
		return mNativeWindow;
	}

	void Sdl3Window::SyncSizeWithNativeWindow()
	{
		int width = 0;
		int height = 0;
		if (!SDL_GetWindowSize(mNativeWindow, &width, &height))
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not retrieve SDL3 window size: %s",
				SDL_GetError());
			return;
		}

		mParams.Width = width;
		mParams.Height = height;
	}

	void Sdl3Window::CreateNativeWindow()
	{
		int32_t defaultWindowFlag = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

		mNativeWindow = SDL_CreateWindow(
			mParams.Title.data(),
			mParams.Width,
			mParams.Height,
			defaultWindowFlag);

		if (nullptr == mNativeWindow)
		{
			UAVPF_LOG(
				Application,
				Error,
				"Could not create SDL3 window: %s",
				SDL_GetError());
		}
	}

	void Sdl3Window::DestroyNativeWindow()
	{
		SDL_DestroyWindow(mNativeWindow);
		mNativeWindow = nullptr;
	}
}
