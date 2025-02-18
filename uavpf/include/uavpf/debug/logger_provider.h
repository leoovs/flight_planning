#pragma once

#include "uavpf/debug/logger.h"
#include "uavpf/debug/log_record.h"

#include <array>
#include <vector>

#define UAVPF_LOG(source, level, fmt, ...) \
	::uavpf::LoggerProvider::Get().Log( \
		source, \
		::uavpf::MakeLogRecord(level, fmt, ##__VA_ARGS__))

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

