#pragma once

#include "uavpf/debug/log_level.h"
#include "uavpf/debug/log_source.h"

#include <string>

namespace uavpf
{
	struct FormattedLog
	{
		LogLevel Level = LogLevel::Unspecified;
		LogSource Source = LogSource::Unspecified;
		std::string Log;
	};
}

