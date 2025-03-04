#pragma once

#include "graphics/graphics_debug_watch.h"

#include <GL/glew.h>

namespace editor
{
	class OglGraphicsDebugWatch final : public GraphicsDebugWatch
	{
	public:
		OglGraphicsDebugWatch();
		~OglGraphicsDebugWatch() override;

		void SetWatcher(GraphicsDebugWatcher watcher) override; 

	private:
		static void NativeCallback(
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* typeErasedWatch);

		void RegisterCallback();
		void UnregisterCallback();

		GraphicsDebugWatcher mWatcher;
	};
}

