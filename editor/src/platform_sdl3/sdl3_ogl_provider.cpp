#include "platform_sdl3/sdl3_ogl_provider.h"

#include <uavpf/uavpf.h>

namespace editor
{
	Sdl3OglProvider::Sdl3OglProvider(Sdl3OglProviderParams params)
		: mParams(std::move(params))
	{
		AcquireNativeContext();
	}

	Sdl3OglProvider::~Sdl3OglProvider()
	{
		ReleaseNativeContext();
	}

	void Sdl3OglProvider::SwapBuffers()
	{
		if (!SDL_GL_SwapWindow(mNativeWindow))
		{
			UAVPF_LOG(
				Application,
				Error,
				"SDL3 OpenGL provider failed to swap buffers: %s",
				SDL_GetError());
		}
	}

	const Sdl3OglProviderParams& Sdl3OglProvider::GetParams() const
	{
		return mParams;
	}

	SDL_Window* Sdl3OglProvider::GetNativeWindow() const
	{
		return mNativeWindow;
	}

	SDL_GLContext Sdl3OglProvider::GetNativeContext() const
	{
		return mNativeContext;
	}

	void Sdl3OglProvider::AcquireNativeContext()
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);	
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);	
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	

		int32_t contextFlags = 0;	
		contextFlags |= mParams.IsContextDebuggable ? SDL_GL_CONTEXT_DEBUG_FLAG : 0;
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

		mNativeWindow = mParams.OutputWindow->GetNativeWindow();
		mNativeContext = SDL_GL_CreateContext(mNativeWindow);

		if (nullptr == mNativeContext)
		{
			UAVPF_LOG(
				Application,
				Error,
				"SDL3 could not provide OpenGL context: %s",
				SDL_GetError());
		}
		UAVPF_LOG(Application, Info, "SDL3 OpenGL provider: context acquired");

		if (GLenum error = glewInit(); GLEW_OK != error)
		{
			UAVPF_LOG(
				Application,
				Error,
				"OpenGL Extension Wrangler failed with error: %s",
				glewGetErrorString(error));
		}
		UAVPF_LOG(Application, Info, "SDL3 OpenGL provider: extensions loaded");
		UAVPF_LOG(Application, Info, "OpenGL vendor: %s", glGetString(GL_VENDOR));
	}

	void Sdl3OglProvider::ReleaseNativeContext()
	{
		SDL_GL_DestroyContext(mNativeContext);
		mNativeContext = nullptr;

		UAVPF_LOG(Application, Trace, "OpenGL context released");
	}
}

