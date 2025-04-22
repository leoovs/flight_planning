#pragma once

struct ImDrawData;

namespace editor
{
	class GraphicsContext;

	class ImGuiGraphicsBackend
	{
	public:
		virtual ~ImGuiGraphicsBackend() = default;

		virtual GraphicsContext* GetContext() const = 0;

		virtual void NewFrame() = 0;
		virtual void RenderDrawData(ImDrawData* data) = 0;
	};
}

