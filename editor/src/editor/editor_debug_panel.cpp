#include "editor/editor_debug_panel.h"

#include <imgui.h>

namespace editor
{
	EditorPanelKind EditorDebugPanel::GetKind() const
	{
		return EditorPanelKind::Debug;
	}

	void EditorDebugPanel::OnImGui()
	{
		ImGui::ShowDemoWindow();
	}
}

