#pragma once

#include <memory>
#include <type_traits>

#include "app/task_executor.h"
#include "event/event_bus.h"
#include "event/event_queue.h"
#include "event/event_subscriber.h"
#include "platform/platform_service.h"
#include "rendering/frame_timer.h"

namespace editor
{
	class App;
	class AppQuitEvent;

	class AppService
	{
	public:
		AppService();
		~AppService();

		template<typename AppT, typename = std::enable_if_t<std::is_base_of_v<App, AppT>>>
		AppService& Bind()
		{
			mRuntimeApp = std::make_unique<AppT>();
			return *this;
		}

		void Run();
		void Render();

		PlatformService* GetPlatform() const;
		Window* GetWindow() const;
		Mouse* GetMouse() const;
		Keyboard* GetKeyboard() const;
		GraphicsContext* GetGraphics() const;
		ImGuiGraphicsBackend* GetImGuiGraphicsBackend() const;

		float GetDeltaTime() const;

		void AddTask(TaskExecutor::TaskFn fn, std::function<void()> onComplete = nullptr);
		
	private:
		void Initialize();

		bool OnAppQuit(const AppQuitEvent& event);

		EventBus mEvents;
		EventDispatcher mEventDispatcher;
		EventQueue mEventQueue;
		EventSubscriber mEventSubscriber;
		
		bool mRunning = false;
		FrameTimer mFrameTimer;
		float mDeltaTime = 0.0f;

		std::shared_ptr<PlatformService> mPlatform;
		std::shared_ptr<Window> mWindow;
		std::shared_ptr<Mouse> mMouse;
		std::shared_ptr<Keyboard> mKeyboard;
		std::shared_ptr<GraphicsContext> mGraphics;
		std::shared_ptr<ImGuiPlatformBackend> mImGuiPlatform;
		std::shared_ptr<ImGuiGraphicsBackend> mImGuiGraphics;

		std::unique_ptr<App> mRuntimeApp;
		TaskExecutor mTasks;
	};
}

