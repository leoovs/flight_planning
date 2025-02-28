#include "event/event_publisher.h"

namespace editor
{
	EventPublisher::EventPublisher() = default;

	EventPublisher::EventPublisher(EventBus& bus)
		: mBus(&bus)
	{
	}

	bool EventPublisher::Publish(
		EventPublishMode mode,
		std::unique_ptr<Event> event)
	{
		return mBus
			? mBus->Publish(mode, std::move(event))
			: false;
	}
}

