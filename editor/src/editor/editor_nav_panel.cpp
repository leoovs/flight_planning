#include "editor/editor_nav_panel.h"
#include "editor/editor_events.h"
#include "editor/editor_panel.h"
#include "editor/path_planner.h"
#include "event/event_publisher.h"
#include "uavpf/nav/nav_resolution.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

namespace editor
{
	EditorNavPanel::EditorNavPanel(PathPlanner& pathPlanner)
		: mPathPlanner(&pathPlanner)
	{}

	EditorPanelKind EditorNavPanel::GetKind() const
	{
		return EditorPanelKind::Nav;
	}

	void EditorNavPanel::Connect(EventBus events)
	{
		mPublisher = EventPublisher(events);
	}

	void EditorNavPanel::OnImGui()
	{
		ImGui::Begin("Path Planner");

		uavpf::experimental::NavResolution res = mPathPlanner->GetResolution();

		ImGui::Text("NavGrid Resolution");
		if (ImGui::DragInt2("##NAV-RES", &res.Width, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			mPathPlanner->SetResolution(res);
			mPublisher.Publish<UpdateNavGridResolutionEvent>(EventPublishMode::Queued, res);
		}

		ImGui::NewLine();

		ImGui::Text("Nav Coordinates");
		for (ptrdiff_t iCheckpoint = 0; iCheckpoint < +CheckpointKind::Count_; iCheckpoint++)
		{
			auto kind = CheckpointKind(iCheckpoint);

			ImGui::PushID(iCheckpoint);

			glm::ivec2 navCoord = mPathPlanner->GetNavCoord(kind);
			bool navCoordUpdated = false;

			if (ImGui::BeginTable("NAV-COORD-DRAG", 3))
			{
				ImGui::TableSetupColumn("#COL0", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("#COL1", ImGuiTableColumnFlags_WidthStretch);
				
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(iCheckpoint ? "End" : "Start"); // TODO: replace with ToString()
				ImGui::TableSetColumnIndex(1);
				navCoordUpdated = navCoordUpdated | ImGui::DragInt("##NAV-COORD-WIDTH", &navCoord.x, 1.0f, 0, res.Width - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
				ImGui::TableSetColumnIndex(2);
				navCoordUpdated = navCoordUpdated | ImGui::DragInt("##NAV-COORD-DEPTH", &navCoord.y, 1.0f, 0, res.Depth - 1, "%d", ImGuiSliderFlags_AlwaysClamp);

				ImGui::EndTable();
			}

			if (navCoordUpdated)
			{
				mPathPlanner->SetNavCoord(kind, navCoord);
				mPublisher.Publish<UpdateCheckpointNavCoord>(EventPublishMode::Queued, navCoord, kind);
			}

			ImGui::PopID();
		}

		ImGui::End();
	}
}

