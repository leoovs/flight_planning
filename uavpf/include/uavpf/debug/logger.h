#pragma once

#include "uavpf/debug/log_emitter.h"
#include "uavpf/debug/log_formatter.h"
#include "uavpf/debug/log_source.h"

#include <memory>

namespace uavpf
{
	class Logger
	{
	public:
		Logger(
			std::shared_ptr<LogFormatter> formatter,
			std::shared_ptr<LogEmitter> emitter,
			LogSource source);

		void Log(const LogRecord& record) const;

	private:
		std::shared_ptr<LogFormatter> mFormatter;
		std::shared_ptr<LogEmitter> mEmitter;
		LogSource mSource = LogSource::Unspecified;
	};
}

