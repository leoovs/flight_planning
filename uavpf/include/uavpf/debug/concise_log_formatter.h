#pragma once

#include "uavpf/debug/log_formatter.h"

namespace uavpf
{
	class ConciseLogFormatter final : public LogFormatter
	{
	public:
		static std::string_view GetLogSourceString(LogSource source);
		static std::string_view GetLogLevelString(LogLevel level);

		~ConciseLogFormatter() override = default;

		FormattedLog Format(
			LogSource source,
			const LogRecord& record) override;
	};
}

