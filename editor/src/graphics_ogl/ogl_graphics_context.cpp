#include "graphics_ogl/ogl_graphics_context.h"

#include <cassert>

#include <GL/glew.h>

namespace editor
{
	OglGraphicsContext::OglGraphicsContext(
		GraphicsContextParams params,
		std::unique_ptr<OglProvider> provider)
		: mParams(std::move(params))
		, mProvider(std::move(provider))
	{
		assert(nullptr != mProvider);
	}

	const GraphicsContextParams& OglGraphicsContext::GetParams() const
	{
		return mParams;
	}

	void OglGraphicsContext::Present()
	{
		mProvider->SwapBuffers();
	}

	void OglGraphicsContext::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

