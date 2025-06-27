#pragma once

#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "runtime/app.h"
#include "platform/platform_events.h"

namespace editor
{
	class EditorApp final : public App	
	{
	public:
		~EditorApp() override = default;

		void Connect(EventBus events) override;
		
		void Update(float dt) override;
		void Render() override;

	private:
		bool OnWindowClosed(const WindowCloseEvent& event);

		EventBus mEvents;
		EventPublisher mPublisher;
		EventSubscriber mSubscriber;
	};
}

