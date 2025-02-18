#include "uavpf/debug/log_record.h"

#include "uavpf/format_str.h"

uavpf::LogRecord uavpf::MakeLogRecord(
	std::string_view sourceFileName,
	int32_t sourceFileLineNumber,
	LogLevel level,
	std::string_view fmt,
	...)
{
	va_list args;
	va_start(args, fmt);
	std::string message = FormatStrV(fmt, args);
	va_end(args);

	LogRecord record;
	record.SourceFileName = sourceFileName;
	record.SourceFileLineNumber = sourceFileLineNumber;
	record.Level = level;
	record.Message = std::move(message);

	return record;
}

