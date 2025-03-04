#pragma once

#include <functional>
#include <string>

namespace editor
{
	struct GraphicsDebugEntry
	{
		std::string Message;
	};

	using GraphicsDebugWatcher = std::function<void(const GraphicsDebugEntry&)>;

	class GraphicsDebugWatch
	{
	public:
		virtual ~GraphicsDebugWatch() = default;

		virtual void SetWatcher(GraphicsDebugWatcher watcher) = 0; 
	};
}

