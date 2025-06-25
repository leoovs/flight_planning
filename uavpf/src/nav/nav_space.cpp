#include "uavpf/nav/nav_space.h"

#include <cassert>

namespace uavpf::experimental
{
	NavSpace::NavSpace(NavResolution resolution)
		: mResolution(resolution)
	{}

	NavResolution NavSpace::GetResolution() const
	{
		return mResolution;
	}

	bool NavSpace::IsSingular() const
	{
		return mResolution.Width == 0 || mResolution.Depth == 0;
	}

	glm::ivec2 NavSpace::FromNavCoord(glm::ivec2 navCoord, glm::ivec2 sourceResolution) const
	{
		glm::vec2 rel = Normalize(navCoord, glm::ivec2(mResolution));
		return rel * glm::vec2(sourceResolution);
	}

	glm::ivec2 NavSpace::ToNavCoord(glm::ivec2 sourceCoord, glm::ivec2 sourceResolution) const
	{
		glm::vec2 rel = Normalize(sourceCoord, sourceResolution);
		return rel * glm::vec2(glm::ivec2(mResolution));
	}

	glm::vec2 NavSpace::Normalize(glm::ivec2 coords, glm::ivec2 resolution) const
	{
		return
		{
			coords.x / resolution.x,
			coords.y / resolution.y,
		};
	}
}

