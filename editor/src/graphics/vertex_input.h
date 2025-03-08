#pragma once

#include <limits>
#include <string>
#include <vector>

#include "graphics/graphics_buffer_slots.h"
#include "graphics/graphics_format.h"

namespace editor
{
	struct VertexAttributeParams
	{
		std::string Name = "Unname vertex attribute";
		GraphicsFormat Format = GraphicsFormat::R32G32B32A32_FLOAT;
		size_t VertexBufferSlotIndex = 0;

		static constexpr auto cAppendAlignedByteOffset =
			std::numeric_limits<size_t>::max();
		size_t ByteOffset = cAppendAlignedByteOffset;
	};

	struct VertexInputParams
	{
		std::string DebugName = "Unnamed VertexInput";

		GraphicsBufferSlots VertexBuffers;
		GraphicsBuffer* IndexBuffer = nullptr;
		std::vector<VertexAttributeParams> VertexAttributes;
	};

	class VertexInput
	{
	public:
		virtual ~VertexInput() = default;

		virtual const VertexInputParams& GetParams() const = 0;
	};
}

