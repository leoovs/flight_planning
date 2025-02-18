#pragma once

#include "uavpf/debug/formatted_log.h"

#include <string>

namespace uavpf
{
	struct LogRecord;

	class LogFormatter
	{
	public:
		virtual ~LogFormatter() = default;

		virtual FormattedLog Format(LogSource source, const LogRecord& record) = 0;
	};
}

