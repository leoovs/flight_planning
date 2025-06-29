#pragma once

#include "runtime/app.h"

namespace editor
{
	void StartUp(AppFactoryFn boundApp);

	template<typename AppT>
	void StartUp()
	{
		StartUp(App::Bind<AppT>());
	}
}

