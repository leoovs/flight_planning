#include "event/event_dispatcher.h"

namespace editor
{
	void EventDispatcher::Unsubscribe(const EventSubscription& subscription)
	{
		mCallbackRegistriesByEventTypeID
			.at(subscription.EventTypeID)
			->Unsubscribe(subscription.SubscriberID);
	}

	bool EventDispatcher::Notify(const Event& event)
	{
		return mCallbackRegistriesByEventTypeID
			.at(typeid(event))
			->Notify(event);
	}

	bool EventDispatcher::Notify(const std::unique_ptr<Event>& event)
	{
		return Notify(*event);
	}
}

