#pragma once

#include <memory>

#include "graphics/graphics_context.h"
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

		GraphicsBuffer* CreateGraphicsBuffer(GraphicsBufferParams params) override;
		void DestroyGraphicsBuffer(GraphicsBuffer* graphicsBuffer) override;

	private:
		GraphicsContextParams mParams;
		std::unique_ptr<OglProvider> mProvider;
	};
}

