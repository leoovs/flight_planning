#pragma once

#include <functional>

#include "event/event_bus.h"

namespace editor
{
	class EventSubscriber
	{
	public:
		template<typename ClassT>
		class MethodSubscriptionChain
		{
		public:
			MethodSubscriptionChain(ClassT& classInstance, EventSubscriber& parent)
				: mClassInstance(classInstance)
				, mParent(parent)
			{
			}

			template<typename EventT>
			MethodSubscriptionChain& SubscribeMethod(
				bool(ClassT::*method)(const EventT& event));

			EventSubscriber& EndClass()
			{
				return mParent.get();
			}
			
		private:
			std::reference_wrapper<ClassT> mClassInstance;
			std::reference_wrapper<EventSubscriber> mParent;
		};

		EventSubscriber() = default;
		EventSubscriber(EventSubscriber&& other) noexcept;
		EventSubscriber(EventBus bus);

		EventSubscriber& operator=(EventSubscriber&& other) noexcept;

		~EventSubscriber();

		EventBus GetBus() const;

		void AddSubscription(EventSubscription subscription);

		template<typename EventT>
		void SubscribeFunction(bool(*fnCallback)(const EventT& event))
		{
			AddSubscription(mBus.Subscribe(fnCallback));
		}

		template<typename ClassT>
		MethodSubscriptionChain<ClassT> BeginClass(ClassT& classInstance)
		{
			return MethodSubscriptionChain<ClassT>(classInstance, *this);
		}

	private:
		void UnsubscribeAll();

		std::vector<EventSubscription> mSubscriptions;
		EventBus mBus;
	};

	template<typename ClassT>
	template<typename EventT>
	EventSubscriber::MethodSubscriptionChain<ClassT>&
	EventSubscriber::MethodSubscriptionChain<ClassT>::SubscribeMethod(
		bool(ClassT::*method)(const EventT& event))
	{
		EventSubscription subscription = mParent.get().GetBus().Subscribe(EventCallback<EventT>(
			[classInstancePtr = &mClassInstance.get(), method](const EventT& event) -> bool
			{
				return std::invoke(method, classInstancePtr, event);
			}
		));
		mParent.get().AddSubscription(std::move(subscription));

		return *this;
	}
}

