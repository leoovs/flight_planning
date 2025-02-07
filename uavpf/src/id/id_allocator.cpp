#include "uavpf/id/id_allocator.h"

#include <cassert>

namespace uavpf
{
	BaseID IDAllocator::Allocate()
	{
		assert(!IsExhausted());

		BaseID id = cBadID;
		if (!mRecycledIDs.empty())
		{
			id = mRecycledIDs.top();
			mRecycledIDs.pop();
		}
		else
		{
			id = mIDGenerator++;
		}

		mActiveIDs.insert(id);

		return id;
	}

	bool IDAllocator::IsActive(BaseID id) const
	{
		return mActiveIDs.count(id) != 0;
	}

	void IDAllocator::Free(BaseID id)
	{
		assert(IsActive(id));

		mActiveIDs.erase(id);
		mRecycledIDs.push(id);
	}

	bool IDAllocator::IsExhausted() const
	{
		return mIDGenerator == cBadID
			&& mRecycledIDs.empty();
	}
}

