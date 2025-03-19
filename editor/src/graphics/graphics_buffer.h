#pragma once

#include <cinttypes>
#include <cstddef>

#include <string>

namespace editor
{
	enum class GraphicsBufferTarget
	{
		None,
		Vertex,
		Index,
		Constant,
	};

	struct GraphicsBufferParams
	{
		std::string DebugName = "Unnamed GraphicsBuffer";

		GraphicsBufferTarget Target = GraphicsBufferTarget::None;

		int64_t StructSize = alignof(max_align_t);
		int64_t StructCount = 1;
	};

	class GraphicsBuffer
	{
	public:
		virtual ~GraphicsBuffer() = default;

		virtual const GraphicsBufferParams& GetParams() const = 0;

		virtual void SetData(const void* data, size_t sizeOfData) = 0;
	};
}

