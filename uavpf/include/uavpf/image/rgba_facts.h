#pragma once

#include <cinttypes>

#include <tuple>

namespace uavpf
{
	class RgbaFacts
	{
	public:
		inline static constexpr uint32_t cMinChannelValue = 0;
		inline static constexpr uint32_t cMaxChannelValue = 255;

		inline static constexpr uint32_t cChannelMask = 0xFF;

		inline static constexpr uint32_t cChannelShift[]
		{
			24,
			16,
			8,
			0
		};

		static constexpr std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
		DecomposeRgba(uint32_t rgba)
		{
			return std::make_tuple(
				(rgba >> 24) & 0xFF,
				(rgba >> 16) & 0xFF,
				(rgba >> 8) & 0xFF,
				(rgba >> 0) & 0xFF);
		}
	};
}

