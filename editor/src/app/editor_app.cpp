#include "app/editor_app.h"

#include "app/app_events.h"
#include "event/event_subscriber.h"

#include <imgui.h>
#include <uavpf/uavpf.h>

namespace editor
{
	void EditorApp::RegisterService(AppService* service)
	{
		mService = service;
		mContext = std::make_unique<AppContext>(service);
	}

	void EditorApp::Bind(EventBus& bus)
	{
		mEventBus = &bus;

		mEventSubscriber = EventSubscriber(bus);
		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClose)
				.SubscribeMethod(&EditorApp::OnWindowResize)
			.EndClass();

		mEventPublisher = EventPublisher(bus);
	}

	void EditorApp::OnRun()
	{
		SetupStates();
		Push(EditorStateKind::Idle);
		GetCurrentState()->OnAttach(this);
	}

	void EditorApp::OnUI()
	{
		ShowDockspace();
		GetCurrentState()->ShowUI();
	}

	void EditorApp::Update()
	{
		if (mStateQueue.size() > 1)
		{
			GetCurrentState()->OnDetach();
			mStateQueue.pop();
			GetCurrentState()->OnAttach(this);
		}
		GetCurrentState()->Update();
	}

	void EditorApp::Render()
	{
		mService->GetGraphics()->ClearColor(nullptr, 0.1f, 0.1f, 0.1f, 1.0f);
		GetCurrentState()->Render();
	}

	void EditorApp::Push(EditorStateKind kind)
	{
		mStateQueue.push(kind);
	}

	EventBus& EditorApp::GetEvents() const
	{
		return *mEventBus;
	}

	AppService* EditorApp::GetService() const
	{
		return mService;
	}

	AppContext* EditorApp::GetContext() const
	{
		return mContext.get();
	}

	void EditorApp::SetupStates()
	{
		std::unique_ptr<EditorState> states[]
		{
			std::make_unique<Editor_Idle>(),
			std::make_unique<Editor_PathBuilder>(),
		};

		for (std::unique_ptr<EditorState>& state : states)
		{
			mStates.at(size_t(state->GetKind())) = std::move(state);
		}
	}

	EditorState* EditorApp::GetCurrentState() const
	{
		return mStates.at(size_t(mStateQueue.front())).get();
	}

	void EditorApp::ShowDockspace()
	{
		ImGuiWindowFlags dockspaceWindowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_MenuBar;

		const ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos);
		ImGui::SetNextWindowSize(vp->WorkSize);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("###EditorDockspaceWindow", nullptr, dockspaceWindowFlags);
		ImGui::PopStyleVar(3);

		GetCurrentState()->ShowMenu();
		ImGui::DockSpace(ImGui::GetID("###EditorDockspace"));

		ImGui::End();
	}

	void EditorApp::ShowMenu()
	{
	}

	bool EditorApp::OnWindowClose(const WindowCloseEvent& event)
	{
		mEventPublisher.Publish<AppQuitEvent>(EventPublishMode::Queued);	
		return true;
	}

	bool EditorApp::OnWindowResize(const WindowResizeEvent& event)
	{
		mService->Render();
		return true;
	}
}

