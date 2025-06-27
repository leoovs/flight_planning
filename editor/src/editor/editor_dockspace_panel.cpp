#include "editor/editor_dockspace_panel.h"

#include <imgui/imgui.h>

#include "editor/editor_panel.h"

namespace editor
{
	EditorPanelKind EditorDockspacePanel::GetKind() const
	{
		return EditorPanelKind::Dockspace;
	}

	void EditorDockspacePanel::OnImGui()
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

		ImGui::End();
	}
}

