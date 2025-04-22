#include "app/editor_app.h"

#include "app/app_events.h"
#include "event/event_subscriber.h"

#include <imgui.h>

namespace editor
{
	void EditorApp::RegisterService(AppService* service)
	{
		mService = service;
	}

	void EditorApp::Bind(EventBus& bus)
	{
		mEventSubscriber = EventSubscriber(bus);
		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClose)
			.EndClass();

		mEventPublisher = EventPublisher(bus);
	}

	void EditorApp::OnUI()
	{
		ImGui::ShowDemoWindow();
	}

	void EditorApp::Render()
	{
		mService->GetGraphics()->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
	}

	bool EditorApp::OnWindowClose(const WindowCloseEvent& event)
	{
		mEventPublisher.Publish<AppQuitEvent>(EventPublishMode::Queued);	
		return true;
	}
}

