#include "uavpf/nav/nav_space.h"

#include <cassert>

namespace uavpf
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
		glm::ivec2 src = rel * glm::vec2(sourceResolution);

		glm::ivec2 clamped
		{
			glm::clamp(src.x, 0, sourceResolution.x - 1),
			glm::clamp(src.y, 0, sourceResolution.y - 1),
		};

		return clamped;
	}

	glm::ivec2 NavSpace::ToNavCoord(glm::ivec2 sourceCoord, glm::ivec2 sourceResolution) const
	{
		glm::vec2 rel = Normalize(sourceCoord, sourceResolution);
		glm::ivec2 navCoord = rel * glm::vec2(glm::ivec2(mResolution));

		glm::ivec2 clamped
		{
			glm::clamp(navCoord.x, 0, mResolution.Width - 1),
			glm::clamp(navCoord.y, 0, mResolution.Depth - 1),
		};

		return clamped;
	}

	glm::vec2 NavSpace::Normalize(glm::ivec2 coords, glm::ivec2 resolution) const
	{
		return
		{
			static_cast<float>(coords.x) / resolution.x,
			static_cast<float>(coords.y) / resolution.y,
		};
	}
}

