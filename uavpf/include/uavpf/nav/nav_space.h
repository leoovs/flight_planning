#pragma once

#include <glm/glm.hpp>

#include "uavpf/nav/nav_resolution.h"

namespace uavpf::experimental
{
	class NavSpace
	{
	public:
		NavSpace(NavResolution resolution);

		NavResolution GetResolution() const;
		bool IsSingular() const;

		glm::ivec2 FromNavCoord(glm::ivec2 navCoord, glm::ivec2 sourceResolution) const;
		glm::ivec2 ToNavCoord(glm::ivec2 sourceCoord, glm::ivec2 sourceResolution) const;

	private:
		glm::vec2 Normalize(glm::ivec2 coords, glm::ivec2 resolution) const;

		NavResolution mResolution;
	};
}

