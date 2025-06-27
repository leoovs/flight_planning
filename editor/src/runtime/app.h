#pragma once

#include <memory>
#include <functional>

#include "event/event_bus.h"

namespace editor
{
	using AppFactoryFn = std::function<std::unique_ptr<class App>()>;

	class App
	{
	public:
		template<typename AppT>
		static AppFactoryFn Bind()
		{
			return []() -> std::unique_ptr<App>
			{
				return std::make_unique<AppT>();
			};
		}

		virtual ~App() = default;

		virtual void Setup() {}
		virtual void Connect(EventBus events) = 0;

		virtual void BeginFrame() {}
		virtual void Update(float dt) = 0;
		virtual void OnImGui() {}
		virtual void Render() = 0;
		virtual void EndFrame() {}

		virtual void ShutDown() {}
	};
}

