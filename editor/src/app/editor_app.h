#pragma once

#include <uavpf/uavpf.h>

#include "app/app.h"
#include "app/app_service.h"
#include "event/event_publisher.h"
#include "platform/platform_events.h"

namespace editor
{
	enum class EditorState
	{
		WaitingOnLoad,
		Operating,
	};

	class EditorApp final : public App
	{
	public:
		void RegisterService(AppService* service) override;
		void Bind(EventBus& bus) override;

		void OnUI() override;
		void Render() override;

	private:
		void ShowDockspace();

		bool OnWindowClose(const WindowCloseEvent& event);
		bool OnPathFound(const class PathFoundEvent& event);

		AppService* mService = nullptr;
		EventSubscriber mEventSubscriber;
		EventPublisher mEventPublisher;

		EditorState mState = EditorState::Operating;
		std::vector<uavpf::NavNode*> mPath;
	};
}

