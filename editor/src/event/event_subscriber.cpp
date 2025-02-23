#include "event/event_subscriber.h"

namespace editor
{
	EventSubscriber::EventSubscriber(EventSubscriber&& other) noexcept
		: mBus(std::exchange(other.mBus, nullptr))
		, mSubscriptions(std::move(other.mSubscriptions))
	{}

	EventSubscriber::EventSubscriber(EventBus& bus)
		: mBus(&bus)
	{
	}

	EventSubscriber& EventSubscriber::operator=(EventSubscriber&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		UnsubscribeAll();
		mBus = std::exchange(other.mBus, nullptr);
		mSubscriptions = std::move(other.mSubscriptions);

		return *this;
	}

	EventSubscriber::~EventSubscriber()
	{
		UnsubscribeAll();
	}

	bool EventSubscriber::HasBus() const
	{
		return nullptr != mBus;
	}

	EventBus& EventSubscriber::GetBus() const
	{
		return *mBus;
	}

	void EventSubscriber::AddSubscription(EventSubscription subscription)
	{
		mSubscriptions.push_back(std::move(subscription));
	}

	void EventSubscriber::UnsubscribeAll()
	{
		if (!HasBus())
		{
			return;
		}

		for (const EventSubscription& subscription : mSubscriptions)
		{
			GetBus().Unsubscribe(subscription);
		}
	}
}

