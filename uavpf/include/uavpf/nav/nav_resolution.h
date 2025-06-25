#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace uavpf::experimental
{
	struct NavResolution
	{
		int32_t Width = 2;
		int32_t Depth = 2;

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

