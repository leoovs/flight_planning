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
		EventBus(EventQueue& events, EventDispatcher& dispatcher);

		void Publish(EventPublishMode mode, std::unique_ptr<Event> event);

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
			mDispatcher.get().Subscribe(std::move(callback));
		}

		void Dispatch();

	private:
		bool Fire(const std::unique_ptr<Event>& event);

		std::reference_wrapper<EventQueue> mEvents;
		std::reference_wrapper<EventDispatcher> mDispatcher;
	};
}

