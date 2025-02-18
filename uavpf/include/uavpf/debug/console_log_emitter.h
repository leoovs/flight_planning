#pragma once

#include "uavpf/debug/log_emitter.h"

namespace uavpf
{
	class ConsoleLogEmitter final : public LogEmitter
	{
	public:
		~ConsoleLogEmitter() override = default;

		void Emit(const FormattedLog& log) override;
	};
}

