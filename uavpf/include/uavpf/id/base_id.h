#pragma once

#include <cinttypes>

#include <limits>

namespace uavpf
{
	using BaseID = uint64_t;

	constexpr BaseID cFirstID = 0;
	constexpr BaseID cBadID = std::numeric_limits<BaseID>::max();
}

