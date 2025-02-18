#include "uavpf/debug/console_log_emitter.h"

#include <iostream>

#include "uavpf/debug/formatted_log.h"

namespace uavpf
{
	void ConsoleLogEmitter::Emit(const FormattedLog& log)
	{
		// Finally, after layers and layers of abstraction we can print
		// something to the console!
		std::cout << log.Log << '\n';
	}
}

