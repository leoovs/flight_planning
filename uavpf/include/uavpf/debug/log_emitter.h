#pragma once

#include "uavpf/debug/formatted_log.h"

#include <string_view>

namespace uavpf
{
	struct LogRecord;

	class LogEmitter
	{
	public:
		virtual ~LogEmitter() = default;

		virtual void Emit(const FormattedLog& log) = 0;
	};
}

