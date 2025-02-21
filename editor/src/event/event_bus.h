#pragma once

#include "event/event.h"
#include "event/event_dispatcher.h"

#include <functional>
#include <memory>
#include <utility>

namespace editor
{
	enum class EventPublishMode
	{
		Queued,
		Immediate,
	};

	class EventQueue;
	class EventDispatcher;

	class EventBus
	{
	public:
		EventBus() = default;
		EventBus(EventQueue& events, EventDispatcher& dispatcher);

		void SetEvents(EventQueue& events);
		void SetDispatcher(EventDispatcher& dispatcher);

		bool Publish(EventPublishMode mode, std::unique_ptr<Event> event);

		template<typename EventT, typename... ArgsT>
		void Publish(EventPublishMode mode, ArgsT&&... args)
		{
			Publish(
				mode,
				std::make_unique<EventT>(std::forward<ArgsT>(args)...));
		}

		template<typename EventT>
		void Subscribe(EventCallback<EventT> callback)
		{
			mDispatcher->Subscribe(std::move(callback));
		}

		void Dispatch();

		bool HasEvents() const;
		bool HasDispatcher() const;

	private:
		bool Fire(const std::unique_ptr<Event>& event);

		EventQueue* mEvents = nullptr;
		EventDispatcher* mDispatcher = nullptr;
	};
}

