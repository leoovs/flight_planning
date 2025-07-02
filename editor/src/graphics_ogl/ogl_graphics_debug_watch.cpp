#include "graphics_ogl/ogl_graphics_debug_watch.h"

#include <GL/glew.h>

namespace editor
{
	OglGraphicsDebugWatch::OglGraphicsDebugWatch()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		RegisterCallback();
	}

	OglGraphicsDebugWatch::~OglGraphicsDebugWatch()
	{
		UnregisterCallback();
	}

	void OglGraphicsDebugWatch::SetWatcher(GraphicsDebugWatcher watcher)
	{
		mWatcher = std::move(watcher);
	}

	void OglGraphicsDebugWatch::NativeCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* typeErasedWatch)
	{
		if (GL_DEBUG_SEVERITY_NOTIFICATION == severity)
		{
			return;
		}

		auto watch = static_cast<const OglGraphicsDebugWatch*>(typeErasedWatch);

		if (watch->mWatcher)
		{
			watch->mWatcher(GraphicsDebugEntry{ std::string(message, length) });
		}
	}

	void OglGraphicsDebugWatch::RegisterCallback()
	{
		GLDEBUGPROC debugProcedure = &OglGraphicsDebugWatch::NativeCallback;
		const void* typeErasedWatch = this;

		glDebugMessageCallback(debugProcedure, typeErasedWatch);
	}

	void OglGraphicsDebugWatch::UnregisterCallback()
	{
		glDebugMessageCallback(nullptr, nullptr);
	}
}

