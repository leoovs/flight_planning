#include "uavpf/debug/logger.h"

#include <cassert>

namespace uavpf
{
	Logger::Logger(
		std::shared_ptr<LogFormatter> formatter,
		std::shared_ptr<LogEmitter> emitter,
		LogSource source)
		: mFormatter(std::move(formatter))
		, mEmitter(std::move(emitter))
		, mSource(source)
	{
		assert(nullptr != mFormatter && "Formatter is required for logger");
		assert(nullptr != mEmitter && "Emitter is required for logger");
	}

	void Logger::Log(const LogRecord& record) const
	{
		mEmitter->Emit(record, mFormatter->Format(record));
	}
}

