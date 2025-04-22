#pragma once

#include "app/app.h"
#include "app/app_service.h"
#include "event/event_publisher.h"
#include "platform/platform_events.h"

namespace editor
{
	class EditorApp final : public App
	{
	public:
		void RegisterService(AppService* service) override;
		void Bind(EventBus& bus) override;

		void OnUI() override;
		void Render() override;

	private:
		bool OnWindowClose(const WindowCloseEvent& event);

		AppService* mService = nullptr;
		EventSubscriber mEventSubscriber;
		EventPublisher mEventPublisher;
	};
}

