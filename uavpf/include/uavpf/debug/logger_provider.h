#pragma once

#include "uavpf/debug/logger.h"

#include <array>
#include <vector>

namespace uavpf
{
	class LoggerProvider
	{
	public:
		static LoggerProvider& Get();

		void HostLogger(
			LogSource source,
			std::shared_ptr<LogFormatter> formatter,
			std::shared_ptr<LogEmitter> emitter);

		void Log(LogSource source, const LogRecord& record);

	private:
		LoggerProvider();
		LoggerProvider(const LoggerProvider& other) = delete;

		std::array<std::vector<Logger>, size_t(LogSource::Count_)> mLoggersBySource;
	};
}

