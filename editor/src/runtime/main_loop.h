#pragma once

#include "event/event_bus.h"
#include "event/event_dispatcher.h"
#include "event/event_queue.h"
#include "event/event_subscriber.h"
#include "graphics/graphics_context.h"
#include "platform/platform_service.h"
#include "runtime/app.h"
#include "tasking/task_scheduler.h"

namespace editor
{
	class MainLoopQuitEvent final : public Event {};

	class MainLoop
	{
	public:
		MainLoop(
			PlatformService* platform,
			TaskScheduler* taskScheduler,
			GraphicsContext* graphics,
			ImGuiPlatformBackend* imguiPlatform,
			ImGuiGraphicsBackend* imguiGraphics,
			AppFactoryFn boundApp);

		void Run();
		void Quit();

	private:
		bool OnMainLoopQuit(const MainLoopQuitEvent& event);

		void PollAndDispatchEvents();
		void BeginFrame();
		void Update(float dt);
		void OnImGui();
		void Render();
		void EndFrame();

		bool mRunning = false;

		PlatformService* mPlatform = nullptr;
		TaskScheduler* mTaskScheduler = nullptr;
		GraphicsContext* mGraphics = nullptr;
		ImGuiPlatformBackend* mImGuiPlatform = nullptr;
		ImGuiGraphicsBackend* mImGuiGraphics = nullptr;
		EventQueue mEventQueue;
		EventDispatcher mEventDispatcher;
		EventBus mEventBus{ mEventQueue, mEventDispatcher };
		EventSubscriber mSubscriber;
		AppFactoryFn mBoundApp;
		std::unique_ptr<App> mAppInstance;
	};
}

