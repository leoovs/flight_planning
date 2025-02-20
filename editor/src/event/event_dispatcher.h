#pragma once

#include "event/event.h"

#include <cassert>

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include <uavpf/id/id_allocator.h>

namespace editor
{
	using EventSubscriberID = uavpf::BaseID;

	template<typename EventT>
	using EventCallback = std::function<bool(const EventT&)>;

	struct EventSubscription
	{
		std::type_index EventTypeID;
		EventSubscriberID SubscriberID = uavpf::cBadID;

		EventSubscription(
			std::type_index eventTypeID,
			EventSubscriberID subscriberID)
			: EventTypeID(eventTypeID)
			, SubscriberID(subscriberID)
		{}
	};

	class IEventCallbackRegistry
	{
	public:
		virtual ~IEventCallbackRegistry() = default;

		virtual bool Notify(const Event& event) = 0;
		virtual void Unsubscribe(EventSubscriberID id) = 0;
	};

	template<typename EventT>
	class EventCallbackRegistry : public IEventCallbackRegistry
	{
	public:
		using SubscriberCallback = EventCallback<EventT>;

		bool Notify(const Event& event) override	
		{
			if (auto actualEventPtr = dynamic_cast<const EventT*>(&event))
			{
				return Dispatch(*actualEventPtr);
			}

			return false;
		}

		void Unsubscribe(EventSubscriberID id) override
		{
			assert(mSubscriberIDRegistry.IsActive(id)
				&& "Trying to unsubscribe subscriber with ID that does not"
				" exist");

			mSubscriberIDRegistry.Free(id);
			mCallbacksBySubscriberID.at(id) = nullptr;
		}

		EventSubscriberID Subscribe(SubscriberCallback callback)
		{
			EventSubscriberID id = mSubscriberIDRegistry.Allocate();

			if (id >= mCallbacksBySubscriberID.size())
			{
				mCallbacksBySubscriberID.resize(id + 1);
			}
			mCallbacksBySubscriberID.at(id) = std::move(callback);

			return id;
		}

	private:
		bool Dispatch(const EventT& event)
		{
			for (const SubscriberCallback& callback : mCallbacksBySubscriberID)
			{
				if (callback && callback(event))
				{
					return true;
				}
			}

			return false;
		}

		uavpf::IDAllocator mSubscriberIDRegistry;
		std::vector<SubscriberCallback> mCallbacksBySubscriberID;
	};

	class EventDispatcher
	{
	public:
		template<typename EventT>
		EventSubscription Subscribe(EventCallback<EventT> callback)
		{
			using Registry = EventCallbackRegistry<EventT>;

			Registry& registry = GetOrCreateCallbackRegistry<EventT>();

			std::type_index eventTypeID = typeid(EventT);
			EventSubscriberID subscriberID = registry.Subscribe(std::move(callback));

			return EventSubscription(eventTypeID, subscriberID);
		}

		void Unsubscribe(const EventSubscription& subscription);
		bool Notify(const Event& event);
		bool Notify(const std::unique_ptr<Event>& event);

	private:
		template<typename EventT>
		EventCallbackRegistry<EventT>& GetOrCreateCallbackRegistry()
		{
			using Registry = EventCallbackRegistry<EventT>;

			std::type_index eventTypeID = typeid(EventT);

			if (mCallbackRegistriesByEventTypeID.count(eventTypeID) == 0)
			{
				auto registry = std::make_unique<Registry>();
				auto registryPtr = registry.get();
				mCallbackRegistriesByEventTypeID.insert(
					std::make_pair(eventTypeID, std::move(registry)));
				return *registryPtr;
			}

			return dynamic_cast<Registry&>(
				*mCallbackRegistriesByEventTypeID.at(eventTypeID));
		}

		std::unordered_map<
			std::type_index,
			std::unique_ptr<IEventCallbackRegistry>
		> mCallbackRegistriesByEventTypeID;		
	};
}

