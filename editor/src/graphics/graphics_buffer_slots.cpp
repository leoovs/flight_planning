#include "graphics/graphics_buffer_slots.h"

#include <cassert>

namespace editor
{
	GraphicsBuffer*& GraphicsBufferSlots::operator[](size_t slotIndex)
	{
		assert(slotIndex < mBuffersBySlot.size());
		return mBuffersBySlot.at(slotIndex);
	}

	GraphicsBuffer* const& GraphicsBufferSlots::operator[](
		size_t slotIndex) const
	{
		assert(slotIndex < mBuffersBySlot.size());
		return mBuffersBySlot.at(slotIndex);
	}

	size_t GraphicsBufferSlots::ConvertIteratorToSlotIndex(
		ConstIterator it) const
	{
		return std::distance(begin(), it);
	}

	GraphicsBufferSlots::Iterator GraphicsBufferSlots::begin()
	{
		return mBuffersBySlot.begin();
	}

	GraphicsBufferSlots::Iterator GraphicsBufferSlots::end()
	{
		return mBuffersBySlot.end();
	}

	GraphicsBufferSlots::ConstIterator GraphicsBufferSlots::begin() const
	{
		return mBuffersBySlot.begin();
	}

	GraphicsBufferSlots::ConstIterator GraphicsBufferSlots::end() const
	{
		return mBuffersBySlot.end();
	}
}

