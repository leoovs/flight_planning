#include "uavpf/debug/concise_log_formatter.h"

#include <cassert>

#include <filesystem>

#include "uavpf/debug/log_record.h"
#include "uavpf/format_str.h"

namespace uavpf
{
	std::string_view ConciseLogFormatter::GetLogSourceString(LogSource source)
	{
		switch (source)
		{
			case LogSource::Unspecified:
				return "Unsp";
			case LogSource::Mock:
				return "Mock";
			case LogSource::Application:
				return "App";
			default:
				assert(false && "Unknown LogSource");
				return "?";
		}
	}

	std::string_view ConciseLogFormatter::GetLogLevelString(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::Unspecified:
				return "Unsp";
			case LogLevel::Trace:
				return "Trace";
			case LogLevel::Note:
				return "Note";
			case LogLevel::Info:
				return "Info";
			case LogLevel::Warning:
				return "Warn";
			case LogLevel::Error:
				return "Err";
			default:
				assert(false && "Unknown LogLevel");
				return "?";
		}
	}

	FormattedLog ConciseLogFormatter::Format(
		LogSource source,
		const LogRecord& record)
	{
		std::filesystem::path fullSourceFilePath(record.SourceFileName);
		std::string shortFilename = fullSourceFilePath.filename().string();

		FormattedLog log;
		log.Source = source;
		log.Level = record.Level;
		log.Log = FormatStr(
			"[%s] [%s] %s :: %s, %" PRId32,
			GetLogSourceString(source).data(),
			GetLogLevelString(record.Level).data(),
			record.Message.data(),
			shortFilename.data(),
			record.SourceFileLineNumber);

		return log;
	}
}

