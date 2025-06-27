#include "runtime/main_loop.h"

#include <imgui/imgui.h>

#include "graphics/imgui_graphics_backend.h"
#include "platform/imgui_platform_backend.h"
#include "rendering/frame_timer.h"

namespace editor
{
	MainLoop::MainLoop(
		PlatformService* platform,
		TaskScheduler* taskScheduler,
		GraphicsContext* graphics,
		ImGuiPlatformBackend* imguiPlatform,
		ImGuiGraphicsBackend* imguiGraphics,
		AppFactoryFn boundApp)
		: mPlatform(platform)
		, mTaskScheduler(taskScheduler)
		, mGraphics(graphics)
		, mImGuiPlatform(imguiPlatform)
		, mImGuiGraphics(imguiGraphics)
		, mBoundApp(std::move(boundApp))
	{
		mSubscriber = EventSubscriber(mEventBus);
		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&MainLoop::OnMainLoopQuit)
			.EndClass();
	}

	void MainLoop::Run()
	{
		mRunning = true;

		mPlatform->BindEvents(mEventBus);

		FrameTimer timer;

		mAppInstance = mBoundApp();
		mAppInstance->Setup();
		mAppInstance->Connect(mEventBus);

		while (mRunning)
		{
			timer.Tick();
			PollAndDispatchEvents();

			BeginFrame();
			{
				Update(timer.GetDeltaTimeSeconds());
				OnImGui();
				Render();
			}
			EndFrame();
		}
	}

	void MainLoop::Quit()
	{
		mRunning = false;
	}

	bool MainLoop::OnMainLoopQuit(const MainLoopQuitEvent& event)
	{
		Quit();
		return true;
	}

	void MainLoop::PollAndDispatchEvents()
	{
		mPlatform->PollEvents();
		mEventBus.Dispatch();
	}

	void MainLoop::BeginFrame()
	{
		mPlatform->BeginFrame();
		mImGuiPlatform->NewFrame();
		mImGuiGraphics->NewFrame();
		ImGui::NewFrame();
		mAppInstance->BeginFrame();
	}

	void MainLoop::Update(float dt)
	{
		mTaskScheduler->Update();
		mAppInstance->Update(dt);
	}

	void MainLoop::OnImGui()
	{
		mAppInstance->OnImGui();
		ImGui::Render();
	}

	void MainLoop::Render()
	{
		mGraphics->ClearColor(nullptr, 0.5f, 0.5f, 0.5f, 1.0f);
		mAppInstance->Render();
		mImGuiGraphics->RenderDrawData(ImGui::GetDrawData());
		mGraphics->Present();
	}

	void MainLoop::EndFrame()
	{
		mAppInstance->EndFrame();
		mPlatform->EndFrame();
	}
}

