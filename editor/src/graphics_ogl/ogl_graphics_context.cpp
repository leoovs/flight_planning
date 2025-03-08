#include "graphics_ogl/ogl_graphics_context.h"
#include "graphics_ogl/ogl_graphics_debug_watch.h"

#include <cassert>

#include <uavpf/uavpf.h>
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

	OglGraphicsDebugWatch* OglGraphicsContext::CreateDebugWatch()
	{
		if (nullptr != mRegisteredDebugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to create multiple graphics debug watches");
			return mRegisteredDebugWatch;
		}

		return mRegisteredDebugWatch = new OglGraphicsDebugWatch();
	}

	void OglGraphicsContext::DestroyDebugWatch(GraphicsDebugWatch* debugWatch)
	{
		if (nullptr == debugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy graphics debug watch which is nullptr");
		}

		if (debugWatch != mRegisteredDebugWatch)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy graphics debug watch context which is"
				" different from registered one");
		}

		delete debugWatch;
		mRegisteredDebugWatch = nullptr;
	}

	OglGraphicsBuffer* OglGraphicsContext::CreateBuffer(
		GraphicsBufferParams params)
	{
		return new OglGraphicsBuffer(std::move(params));
	}

	void OglGraphicsContext::DestroyBuffer(
		GraphicsBuffer* graphicsBuffer)
	{
		delete graphicsBuffer;
	}

	OglVertexInput* OglGraphicsContext::CreateVertexInput(VertexInputParams params)
	{
		return new OglVertexInput(std::move(params));
	}

	void OglGraphicsContext::DestroyVertexInput(VertexInput* vertexInput)
	{
		if (nullptr == vertexInput)
		{
			UAVPF_LOG(
				Application,
				Warning,
				"Trying to destroy nullptr vertex input");
		}
		delete vertexInput;
	}

	void OglGraphicsContext::SetVertexInput(VertexInput* vertexInput)
	{
		if (nullptr == vertexInput)
		{
			glBindVertexArray(0);
			mBoundVertexInput = nullptr;
			return;
		}

		mBoundVertexInput = dynamic_cast<OglVertexInput*>(vertexInput);
		glBindVertexArray(mBoundVertexInput->GetNativeVertexArray());
	}

	OglVertexInput* OglGraphicsContext::GetVertexInput() const
	{
		return mBoundVertexInput;
	}
}

