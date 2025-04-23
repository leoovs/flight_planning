#include "app/app_service.h"

#include "app/app.h"
#include "app/app_events.h"

#include <imgui.h>

namespace editor
{
	AppService::AppService()
	{
		Initialize();
	}

	void AppService::Run()
	{
		mTasks.Start();
		mRunning = true;

		mRuntimeApp->Bind(mEvents);
		mRuntimeApp->RegisterService(this);
		mRuntimeApp->OnRun();

		while (mRunning)
		{
			mFrameTimer.Tick();
			mDeltaTime = mFrameTimer.GetDeltaTimeSeconds();

			mPlatform->PollEvents();
			mEvents.Dispatch();

			mRuntimeApp->Update();

			mImGuiPlatform->NewFrame();
			mImGuiGraphics->NewFrame();
			ImGui::NewFrame();
			mRuntimeApp->OnUI();
			ImGui::Render();

			mRuntimeApp->Render();

			mImGuiGraphics->RenderDrawData(ImGui::GetDrawData());
			mGraphics->Present();
		}

		mRuntimeApp->OnQuit();
		mTasks.Finish();
	}

	PlatformService* AppService::GetPlatform() const
	{
		return mPlatform.get();
	}

	Window* AppService::GetWindow() const
	{
		return mWindow.get();
	}

	Mouse* AppService::GetMouse() const
	{
		return mMouse.get();
	}

	Keyboard* AppService::GetKeyboard() const
	{
		return mKeyboard.get();
	}

	GraphicsContext* AppService::GetGraphics() const
	{
		return mGraphics.get();
	}
	
	float AppService::GetDeltaTime() const
	{
		return mDeltaTime;
	}

	void AppService::AddTask(TaskExecutor::TaskFn fn, std::function<void()> onComplete)
	{
		mTasks.Execute(fn, onComplete);
	}

	void AppService::Initialize()
	{
		mEvents = EventBus(mEventQueue, mEventDispatcher);

		mPlatform.reset(CreatePlatformService(), DestroyPlatformService);
		mPlatform->BindEvents(mEvents);

		mWindow.reset(
			mPlatform->CreateWindow(),
			[this](Window* w) { mPlatform->DestroyWindow(w); }
		);

		mMouse.reset(
			mPlatform->CreateMouse(),
			[this](Mouse* m) { mPlatform->DestroyMouse(m); }
		);

		mKeyboard.reset(
			mPlatform->CreateKeyboard(),
			[this](Keyboard* k) { mPlatform->DestroyKeyboard(k); }
		);

		GraphicsContextParams graphicsParams;
		graphicsParams.Backend = GraphicsBackend::OpenGL;
		graphicsParams.OutputWindow = mWindow.get();
		graphicsParams.Kind = GraphicsContextKind::Optimal;

		mGraphics.reset(
			mPlatform->CreateGraphicsContext(std::move(graphicsParams)),
			[this](GraphicsContext* g) { mPlatform->DestroyGraphicsContext(g); }
		);

		ImGui::CreateContext();

		mImGuiPlatform.reset(
			mPlatform->CreateImGuiBackend(),
			[this](ImGuiPlatformBackend* b) { mPlatform->DestroyImGuiBackend(b); }
		);

		mImGuiGraphics.reset(
			mImGuiPlatform->CreateGraphicsBackend(mGraphics.get()),
			[this](ImGuiGraphicsBackend* b) { mImGuiPlatform->DestroyGraphicsBackend(b); }
		);

		mEventSubscriber = EventSubscriber(mEvents);
		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&AppService::OnAppQuit)
			.EndClass();
	}

	bool AppService::OnAppQuit(const AppQuitEvent& event)
	{
		mRunning = false;
		return true;
	}
}

