#include "event/event_dispatcher.h"

namespace editor
{
	void EventDispatcher::Unsubscribe(const EventSubscription& subscription)
	{
		if (0 == mCallbackRegistriesByEventTypeID.count(subscription.EventTypeID))
		{
			return;
		}

		mCallbackRegistriesByEventTypeID
			.at(subscription.EventTypeID)
			->Unsubscribe(subscription.SubscriberID);
	}

	bool EventDispatcher::Notify(const Event& event)
	{
		if (0 == mCallbackRegistriesByEventTypeID.count(typeid(event)))
		{
			return false;
		}

		return mCallbackRegistriesByEventTypeID
			.at(typeid(event))
			->Notify(event);
	}

	bool EventDispatcher::Notify(const std::unique_ptr<Event>& event)
	{
		return Notify(*event);
	}
}

