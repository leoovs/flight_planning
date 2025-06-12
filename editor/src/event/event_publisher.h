#pragma once

#include "event/event_bus.h"

namespace editor
{
	class EventPublisher
	{
	public:
		EventPublisher();
		EventPublisher(EventBus bus);

		template<typename EventT, typename... ArgsT>
		bool Publish(EventPublishMode mode, ArgsT&&... args)
		{
			return mBus.Publish<EventT>(mode, std::forward<ArgsT>(args)...);
		}

		bool Publish(EventPublishMode mode, std::unique_ptr<Event> event);

	private:
		EventBus mBus;
	};
}

