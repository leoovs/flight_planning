#include <uavpf/uavpf.h>

int main()
{
	auto formatter = std::make_shared<uavpf::ConciseLogFormatter>();
	auto emitter = std::make_shared<uavpf::ConsoleLogEmitter>();

	uavpf::LoggerProvider::Get()
		.HostLogger(
			uavpf::LogSource::Application,
			std::move(formatter),
			std::move(emitter));

	UAVPF_LOG(Application, Trace, "Use this for USUAL things");
	UAVPF_LOG(Application, Note, "Use this to CLARIFY things");
	UAVPF_LOG(Application, Info, "Use this when something GOOD happends");
	UAVPF_LOG(Application, Warning, "Use this when something SUSPICIOUS happens");
	UAVPF_LOG(Application, Error, "Use this when something (VERY) BAD happens");
}

