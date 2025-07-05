#include <uavpf/uavpf.h>

#include "runtime/start_up.h"
#include "editor/editor_app.h"

int main()
{
	auto formatter = std::make_unique<uavpf::ConciseLogFormatter>();
	auto emitter = std::make_unique<uavpf::ConsoleLogEmitter>();

	uavpf::LoggerProvider::Get().HostLogger(
		uavpf::LogSource::Application,
		std::move(formatter),
		std::move(emitter));

	editor::StartUp<editor::EditorApp>();
}

