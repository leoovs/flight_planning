#pragma once

#include <array>

namespace editor
{
	class GraphicsBuffer;

	class GraphicsBufferSlots
	{
	public:
		static constexpr size_t cMaxSlots = 8;

		using Container = std::array<GraphicsBuffer*, cMaxSlots>;
		using Iterator = Container::iterator;
		using ConstIterator = Container::const_iterator;

		GraphicsBuffer*& operator[](size_t slotIndex);
		GraphicsBuffer* const& operator[](size_t slotIndex) const;

		size_t ConvertIteratorToSlotIndex(ConstIterator it) const;

		Iterator begin();
		Iterator end();

		ConstIterator begin() const;
		ConstIterator end() const;

	private:
		Container mBuffersBySlot = {};
	};
}

