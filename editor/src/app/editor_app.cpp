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
		mEventSubscriber = EventSubscriber(bus);
		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClose)
			.EndClass();

		mEventPublisher = EventPublisher(bus);
	}

	void EditorApp::OnUI()
	{
		ShowDockspace();
	}

	void EditorApp::Render()
	{
		mService->GetGraphics()->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
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

		ImGui::DockSpace(ImGui::GetID("###EditorDockspace"));
		ShowMenu();

		ImGui::End();
	}

	void EditorApp::ShowMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open TIFF map", nullptr))
				{
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	bool EditorApp::OnWindowClose(const WindowCloseEvent& event)
	{
		mEventPublisher.Publish<AppQuitEvent>(EventPublishMode::Queued);	
		return true;
	}
}

