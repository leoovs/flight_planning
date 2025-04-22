#include "platform_sdl3/sdl3_imgui_platform_backend.h"

#include <imgui_impl_sdl3.h>

#include "graphics/graphics_context.h"
#include "graphics_ogl/ogl_graphics_context.h"
#include "graphics_ogl/ogl_imgui_graphics_backend.h"
#include "platform_sdl3/sdl3_ogl_provider.h"

namespace editor
{
	ImGuiGraphicsBackend* Sdl3ImGuiPlatformBackend::CreateGraphicsBackend(GraphicsContext* context)
	{
		switch (context->GetParams().Backend)
		{
			case GraphicsBackend::None:
				return nullptr;
			case GraphicsBackend::OpenGL:
				return CreateOglGraphicsBackend(dynamic_cast<OglGraphicsContext*>(context));
			default:
				return nullptr;
		}
	}

	void Sdl3ImGuiPlatformBackend::DestroyGraphicsBackend(ImGuiGraphicsBackend* backend)
	{
		GraphicsContext* context = backend->GetContext();
		switch (context->GetParams().Backend)
		{
			case GraphicsBackend::None:
				return;
			case GraphicsBackend::OpenGL:
				DestroyOglGraphicsBackend(dynamic_cast<OglImGuiGraphicsBackend*>(backend));
			default:
				return;
		}
	}

	void Sdl3ImGuiPlatformBackend::NewFrame()
	{
		ImGui_ImplSDL3_NewFrame();
	}
	
	void Sdl3ImGuiPlatformBackend::HandleNativeEvent(const SDL_Event* event)
	{
		ImGui_ImplSDL3_ProcessEvent(event);
	}

	OglImGuiGraphicsBackend* Sdl3ImGuiPlatformBackend::CreateOglGraphicsBackend(OglGraphicsContext* context)
	{
		auto provider = dynamic_cast<Sdl3OglProvider*>(context->GetProvider()); 
		ImGui_ImplSDL3_InitForOpenGL(provider->GetNativeWindow(), provider->GetNativeContext());
		return new OglImGuiGraphicsBackend(context);
	}

	void Sdl3ImGuiPlatformBackend::DestroyOglGraphicsBackend(OglImGuiGraphicsBackend* backend)
	{
		delete backend;
		ImGui_ImplSDL3_Shutdown();
	}
}

