#pragma once

#include <memory>

#include "graphics/graphics_context.h"
#include "graphics_ogl/ogl_graphics_buffer.h"
#include "graphics_ogl/ogl_graphics_debug_watch.h"
#include "graphics_ogl/ogl_provider.h"

namespace editor
{
	class OglGraphicsContext final : public GraphicsContext
	{
	public:
		OglGraphicsContext(
			GraphicsContextParams params,
			std::unique_ptr<OglProvider> provider);

		virtual const GraphicsContextParams& GetParams() const override;

		void Present() override;
		void ClearColor(float r, float g, float b, float a) override;

		OglGraphicsDebugWatch* CreateDebugWatch() override;
		void DestroyDebugWatch(GraphicsDebugWatch* debugWatch) override;

		OglGraphicsBuffer* CreateBuffer(GraphicsBufferParams params) override;
		void DestroyBuffer(GraphicsBuffer* graphicsBuffer) override;

	private:
		GraphicsContextParams mParams;
		std::unique_ptr<OglProvider> mProvider;

		OglGraphicsDebugWatch* mRegisteredDebugWatch = nullptr;
	};
}

