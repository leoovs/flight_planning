#pragma once

#include <string_view>

namespace uavpf
{
	struct LogRecord;

	class LogEmitter
	{
	public:
		virtual ~LogEmitter() = default;

		virtual void Emit(const LogRecord& record, std::string_view log) = 0;
	};
}

