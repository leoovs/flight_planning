#pragma once

#include "uavpf/debug/log_level.h"

#include <cinttypes>

#include <string>

namespace uavpf
{
	struct LogRecord
	{
		std::string_view SourceFileName;
		int32_t SourceFileLineNumber;
		std::string Message;
		LogLevel Level = LogLevel::Unspecified;
	};

	LogRecord MakeLogRecord(
		std::string_view sourceFileName,
		int32_t sourceFileLineNumber,
		LogLevel level,
		std::string_view fmt,
		...);
}

