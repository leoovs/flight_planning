#pragma once

#include <string_view>

namespace uavpf
{
	struct FormattedLog;

	class LogEmitter
	{
	public:
		virtual ~LogEmitter() = default;

		virtual void Emit(const FormattedLog& log) = 0;
	};
}

