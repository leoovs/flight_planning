#pragma once

#include "graphics/imgui_graphics_backend.h"
#include "graphics_ogl/ogl_graphics_context.h"

namespace editor
{
	class OglImGuiGraphicsBackend final : public ImGuiGraphicsBackend
	{
	public:
		OglImGuiGraphicsBackend(OglGraphicsContext* context);
		~OglImGuiGraphicsBackend() override;

		OglGraphicsContext* GetContext() const override;
		void NewFrame() override;
		void RenderDrawData(ImDrawData* data) override;

	private:
		void InitNativeBackend();
		void ShutDownNativeBackend();

		OglGraphicsContext* mContext = nullptr;
	};
}
 
