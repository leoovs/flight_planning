#pragma once

#include <cinttypes>

namespace editor
{
	struct SubresourceRegion
	{
		int32_t OffsetX = 0;
		int32_t OffsetY = 0;
		int32_t OffsetZ = 0;
		int32_t Width = 0;
		int32_t Height = 0;
		int32_t Depth = 0;
		int32_t ArrayIndex = 0;
		int32_t MipLevelIndex = 0;
	};
}

