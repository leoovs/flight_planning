#pragma once

#include <cassert>
#include <cstdint>

#include <glm/glm.hpp>

#include "uavpf/image/tiff_tag.h"

namespace uavpf
{
	class TiffTagAccessor
	{
	public:
		virtual ~TiffTagAccessor() = default;

		virtual TiffTag GetTag() const = 0;

		virtual glm::mat4 AsMatrix() const = 0;
	};
}

