#include "event/event_queue.h"

#include <cassert>

#include "event/event.h"

namespace editor
{
	void EventQueue::Enqueue(std::unique_ptr<Event> event)
	{
		mEnqeuedEvents.push(std::move(event));
	}

	std::unique_ptr<Event> EventQueue::Dequeue()
	{
		assert(Any() && "Trying to dequeue from EventQueue that is empty");

		std::unique_ptr<Event> event = std::move(mEnqeuedEvents.front());
		mEnqeuedEvents.pop();

		return event;
	}

	bool EventQueue::Any() const
	{
		return !mEnqeuedEvents.empty();
	}
}

