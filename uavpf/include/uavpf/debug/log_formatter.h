#pragma once

#include <string>

namespace uavpf
{
	struct LogRecord;

	class LogFormatter
	{
	public:
		virtual ~LogFormatter() = default;

		virtual std::string Format(const LogRecord& record) = 0;
	};
}

