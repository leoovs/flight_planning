#include "uavpf/debug/logger_provider.h"

namespace uavpf
{
	LoggerProvider& LoggerProvider::Get()
	{
		static LoggerProvider sInstance;
		return sInstance;
	}

	void LoggerProvider::HostLogger(
		LogSource source,
		std::shared_ptr<LogFormatter> formatter,
		std::shared_ptr<LogEmitter> emitter)
	{
		auto key = static_cast<size_t>(source);

		mLoggersBySource
			.at(key)
			.emplace_back(
				std::move(formatter),
				std::move(emitter),
				source);
	}

	void LoggerProvider::Log(LogSource source, const LogRecord& record)
	{
		auto sourceAsKey = static_cast<size_t>(source);

		for (const Logger& responsibleLogger : mLoggersBySource.at(sourceAsKey))
		{
			responsibleLogger.Log(record);
		}
	}

	LoggerProvider::LoggerProvider() = default;
}

