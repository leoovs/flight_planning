#include "event/event_bus.h"

#include "event/event_dispatcher.h"
#include "event/event_queue.h"

namespace editor
{
	EventBus::EventBus(EventQueue& events, EventDispatcher& dispatcher)
		: mEvents(events)
		, mDispatcher(dispatcher)
	{}

	void EventBus::Publish(EventPublishMode mode, std::unique_ptr<Event> event)
	{
		switch (mode)
		{
			case EventPublishMode::Queued:
				mEvents.get().Enqueue(std::move(event));
				break;
			case EventPublishMode::Immediate:
				static_cast<void>(Fire(std::move(event)));
				break;
		}
	}

	void EventBus::Dispatch()
	{
		EventQueue leftover;
		EventQueue& pending = mEvents.get();

		while (pending.Any())
		{
			std::unique_ptr<Event> event = pending.Dequeue();
			if (!Fire(event))
			{
				leftover.Enqueue(std::move(event));
			}
		}

		// By now there are two possible outcomes:
		// 1. pending and leftover are both empty, so this assignment does
		// nothing.
		// 2. pending is empty and leftover is not, in such a case we simply
		// move events to pending again.
		pending = std::move(leftover);
	}

	bool EventBus::Fire(const std::unique_ptr<Event>& event)
	{
		return mDispatcher.get().Notify(event);
	}
}

