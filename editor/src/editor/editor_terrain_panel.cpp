#include "editor/editor_terrain_panel.h"

#include <imgui/imgui.h>

#include "editor/editor_panel.h"

namespace editor
{
	EditorPanelKind EditorTerrainPanel::GetKind() const
	{
		return EditorPanelKind::Terrain;
	}

	void EditorTerrainPanel::OnImGui()
	{
		if (ImGui::Begin("Terrain"))
		{
			ImGui::Text("");
		}
		ImGui::End();
	}
}

