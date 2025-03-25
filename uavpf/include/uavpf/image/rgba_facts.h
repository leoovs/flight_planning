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
			uint32_t r = (rgba >> cChannelShift[0]) & cChannelMask; 
			uint32_t g = (rgba >> cChannelShift[1]) & cChannelMask; 
			uint32_t b = (rgba >> cChannelShift[2]) & cChannelMask; 
			uint32_t a = (rgba >> cChannelShift[3]) & cChannelMask; 

			return std::make_tuple(r, g, b, a);
		}
	};
}

