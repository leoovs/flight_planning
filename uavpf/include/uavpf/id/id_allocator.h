#pragma once

#include "uavpf/id/base_id.h"

#include <stack>
#include <unordered_set>
#include <vector>

namespace uavpf
{
	class IDAllocator
	{
	public:
		BaseID Allocate();
		bool IsActive(BaseID id) const;
		void Free(BaseID id);

		bool IsExhausted() const;

	private:
		BaseID mIDGenerator = cFirstID;
		std::unordered_set<BaseID> mActiveIDs;
		std::stack<BaseID, std::vector<BaseID>> mRecycledIDs;
	};
}

