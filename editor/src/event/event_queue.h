#pragma once

#include <memory>
#include <queue>

namespace editor
{
	class Event;

	class EventQueue
	{
	public:
		void Enqueue(std::unique_ptr<Event> event);
		std::unique_ptr<Event> Dequeue();

		bool Any() const;

	private:
		std::queue<std::unique_ptr<Event>> mEnqeuedEvents;
	};
}

