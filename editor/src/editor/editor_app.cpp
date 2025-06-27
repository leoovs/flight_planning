#include "editor/editor_app.h"

#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "runtime/main_loop.h"

namespace editor
{
	void EditorApp::Connect(EventBus events)
	{
		mEvents = events;
		mPublisher = EventPublisher(events);
		mSubscriber = EventSubscriber(events);

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClosed)
			.EndClass();
	}

	void EditorApp::Update(float dt)
	{
	}

	void EditorApp::Render()
	{
	}

	bool EditorApp::OnWindowClosed(const WindowCloseEvent& event)
	{
		mPublisher.Publish<MainLoopQuitEvent>(EventPublishMode::Queued);
		return true;
	}
}

