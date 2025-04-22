#pragma once

#include "graphics/imgui_graphics_backend.h"

namespace editor
{
	class GraphicsContext;

	class ImGuiPlatformBackend
	{
	public:
		virtual ~ImGuiPlatformBackend() = default;

		virtual ImGuiGraphicsBackend* CreateGraphicsBackend(GraphicsContext* graphics) = 0;
		virtual void DestroyGraphicsBackend(ImGuiGraphicsBackend* backend) = 0;

		virtual void NewFrame() = 0;
	};
}

