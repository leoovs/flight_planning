#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace uavpf
{
	struct NavCell
	{
		size_t Index = 0;
		glm::ivec2 NavCoords{ 0, 0 };
		float RelativeHeight = 0.0f;
	};
}

