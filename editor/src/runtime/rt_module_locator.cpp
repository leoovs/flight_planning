#include "runtime/rt_module_locator.h"

namespace editor
{
	RtModuleLocator& RtModuleLocator::GetInstance()
	{
		static RtModuleLocator sInstance;
		return sInstance;
	}
}

