#pragma once

#include "graphics/imgui_graphics_backend.h"
#include "platform/imgui_platform_backend.h"

union SDL_Event;

namespace editor
{
	class OglGraphicsContext;
	class OglImGuiGraphicsBackend;

	class Sdl3ImGuiPlatformBackend final : public ImGuiPlatformBackend
	{
	public:
		ImGuiGraphicsBackend* CreateGraphicsBackend(GraphicsContext* graphics) override;
		void DestroyGraphicsBackend(ImGuiGraphicsBackend* backend) override;

		void NewFrame() override;

		void HandleNativeEvent(const SDL_Event* event);

	private:
		OglImGuiGraphicsBackend* CreateOglGraphicsBackend(OglGraphicsContext* context);
		void DestroyOglGraphicsBackend(OglImGuiGraphicsBackend* backend);
	};
}
