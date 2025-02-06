#pragma once

#include "uavpf/config.h"

#include <cinttypes>

namespace uavpf
{
	struct Version
	{
		int32_t Major;
		int32_t Minor;
		int32_t Patch;
	};

	Version GetCurrentVersion();
}

