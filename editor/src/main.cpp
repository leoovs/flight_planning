#include <uavpf/uavpf.h>

#include "event/event_bus.h"
#include "event/event_queue.h"
#include "event/event_subscriber.h"
#include "platform/platform_events.h"
#include "platform/platform_service.h"
#include "uavpf/debug/concise_log_formatter.h"

namespace editor
{
	class TestApplication
	{
	public:
		TestApplication()
		{
			mEvents = EventBus(mEventQueue, mDispatcher);
			mSubscriber = EventSubscriber(mEvents);

			mSubscriber
				.BeginClass(*this)
					.SubscribeMethod(&TestApplication::OnWindowClose)
					.SubscribeMethod(&TestApplication::OnWindowResize)
					.SubscribeMethod(&TestApplication::OnMouseButtonDown)
					.SubscribeMethod(&TestApplication::OnMouseButtonUp)
				.EndClass();

			mPlatform = CreatePlatformService();
			mPlatform->BindEvents(mEvents);

			mWindow = mPlatform->CreateWindow();
			mWindow->SetTitle("Editor");

			mMouse = mPlatform->CreateMouse();
		}

		~TestApplication()
		{
			mPlatform->DestroyMouse(mMouse);
			mMouse = nullptr;

			mPlatform->DestroyWindow(mWindow);
			mWindow = nullptr;

			DestroyPlatformService(mPlatform);
			mPlatform = nullptr;
		}

		void StartMainLoop()
		{
			mRunning = true;
			while (mRunning)
			{
				mPlatform->BeginFrame();

				mPlatform->PollEvents();
				mEvents.Dispatch();

				mPlatform->EndFrame();
			}
		}

	private:
		bool OnWindowClose(const WindowCloseEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Window '%s' closed!",
				event.ClosedWindow->GetTitle().data());

			mRunning = false;

			return true;
		}

		bool OnWindowResize(const WindowResizeEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Window '%s' is resized: %dx%d",
				event.ResizedWindow->GetTitle().data(),
				event.ResizedWindow->GetWidth(),
				event.ResizedWindow->GetHeight());

			return true;
		}

		bool OnMouseButtonDown(const MouseButtonDownEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Button: '%i' is down",
				event.ButtonDown);
			return true;
		}

		bool OnMouseButtonUp(const MouseButtonUpEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Button: '%i' is up",
				event.ButtonUp);
			return true;
		}

		bool mRunning = true;

		// Event-system
		EventQueue mEventQueue;
		EventDispatcher mDispatcher;
		EventBus mEvents;
		EventSubscriber mSubscriber;

		// Platform
		PlatformService* mPlatform = nullptr;
		Window* mWindow = nullptr;
		Mouse* mMouse = nullptr;
	};
}
int main()
{
	auto formatter = std::make_unique<uavpf::ConciseLogFormatter>();
	auto emitter = std::make_unique<uavpf::ConsoleLogEmitter>();

	uavpf::LoggerProvider::Get().HostLogger(
		uavpf::LogSource::Application,
		std::move(formatter),
		std::move(emitter));

	editor::TestApplication().StartMainLoop();
}

