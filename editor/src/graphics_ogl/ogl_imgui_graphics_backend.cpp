#include "graphics_ogl/ogl_imgui_graphics_backend.h"

#include <imgui_impl_opengl3.h>

namespace editor
{
	OglImGuiGraphicsBackend::OglImGuiGraphicsBackend(OglGraphicsContext* context)
		: mContext(context)
	{
		InitNativeBackend();
	}

	OglImGuiGraphicsBackend::~OglImGuiGraphicsBackend()
	{
		ShutDownNativeBackend();
	}

	OglGraphicsContext* OglImGuiGraphicsBackend::GetContext() const
	{
		return mContext;
	}

	void OglImGuiGraphicsBackend::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void OglImGuiGraphicsBackend::RenderDrawData(ImDrawData* data)
	{
		ImGui_ImplOpenGL3_RenderDrawData(data);
	}

	void OglImGuiGraphicsBackend::InitNativeBackend()
	{
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void OglImGuiGraphicsBackend::ShutDownNativeBackend()
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
}
