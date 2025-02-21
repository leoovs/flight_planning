#include "event/event_bus.h"

#include <cassert>

#include "event/event_dispatcher.h"
#include "event/event_queue.h"

namespace editor
{
	EventBus::EventBus(EventQueue& events, EventDispatcher& dispatcher)
		: mEvents(&events)
		, mDispatcher(&dispatcher)
	{}

	void EventBus::SetEvents(EventQueue& events)
	{
		mEvents = &events;
	}

	void EventBus::SetDispatcher(EventDispatcher& dispatcher)
	{
		mDispatcher = &dispatcher;
	}

	bool EventBus::Publish(EventPublishMode mode, std::unique_ptr<Event> event)
	{
		switch (mode)
		{
			case EventPublishMode::Queued:
				return HasEvents()
					? mEvents->Enqueue(std::move(event)), true
					: false;
			case EventPublishMode::Immediate:
				return Fire(std::move(event));
		}

		assert(false && "Invalid EventPublishMode enum value");
		return false;
	}

	void EventBus::Dispatch()
	{
		if (!HasEvents() || !HasDispatcher())
		{
			return;
		}

		EventQueue leftover;
		EventQueue& pending = *mEvents;

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

	bool EventBus::HasEvents() const
	{
		return nullptr != mEvents;
	}

	bool EventBus::HasDispatcher() const
	{
		return nullptr != mDispatcher;
	}

	bool EventBus::Fire(const std::unique_ptr<Event>& event)
	{
		if (!HasDispatcher())
		{
			return false;
		}

		return mDispatcher->Notify(event);
	}
}

