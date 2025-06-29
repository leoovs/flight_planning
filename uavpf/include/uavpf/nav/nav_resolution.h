#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace uavpf::experimental
{
	struct NavResolution
	{
		int32_t Width = 0;
		int32_t Depth = 0;

		explicit constexpr operator glm::ivec2() const
		{
			return
			{
				Width,
				Depth,
			};
		}
	};

}

