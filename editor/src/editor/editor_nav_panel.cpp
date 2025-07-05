#include "editor/editor_nav_panel.h"
#include "editor/editor_events.h"
#include "editor/editor_panel.h"
#include "editor/path_planner.h"
#include "event/event_bus.h"
#include "event/event_publisher.h"
#include "uavpf/nav/nav_resolution.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace editor
{
	EditorNavPanel::EditorNavPanel(const PathPlanner& pathPlanner, const NavNetwork& navNetwork)
		: mPathPlanner(&pathPlanner)
		, mNavNetwork(&navNetwork)
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
		ImGui::Begin("Solver");

		bool disabled = mPathPlanner->IsBuildingPath();
		if (disabled)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		}

		uavpf::experimental::NavResolution res = mNavNetwork->GetResolution();

		ImGui::Text("Nav Grid resolution");
		if (ImGui::DragInt2("##NAV-RES", &res.Width, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			mPublisher.Publish<UpdateNavGridResolutionEvent>(
				EventPublishMode::Immediate,
				res);
		}

		ImGui::NewLine();

		ImGui::Text("Nav coordinates");
		for (ptrdiff_t iCheckpoint = 0; iCheckpoint < +CheckpointKind::Count_; iCheckpoint++)
		{
			auto kind = CheckpointKind(iCheckpoint);

			ImGui::PushID(iCheckpoint);

			glm::ivec2 navCoord = mPathPlanner->GetNavCoord(kind);
			bool navCoordUpdated = false;

			if (ImGui::BeginTable("NAV-COORD-DRAG", 3))
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(iCheckpoint ? "End" : "Start"); // TODO: replace with ToString()
				ImGui::TableSetColumnIndex(1);
				navCoordUpdated = navCoordUpdated | ImGui::DragInt("##NAV-COORD-WIDTH", &navCoord.x, 1, 0, res.Width, "%d", ImGuiSliderFlags_AlwaysClamp);
				ImGui::TableSetColumnIndex(2);
				navCoordUpdated = navCoordUpdated | ImGui::DragInt("##NAV-COORD-DEPTH", &navCoord.y, 1, 0, res.Depth, "%d", ImGuiSliderFlags_AlwaysClamp);

				ImGui::EndTable();
			}

			if (navCoordUpdated)
			{
				mPublisher.Publish<UpdateCheckpointNavCoordEvent>(
					EventPublishMode::Immediate,
					navCoord,
					kind);
			}

			ImGui::PopID();
		}

		if (ImGui::Button("Build"))
		{
			mPublisher.Publish<BuildPathEvent>(EventPublishMode::Queued);
		}

		if (disabled)
		{
			ImGui::PopStyleVar();
			ImGui::PopItemFlag();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			mPublisher.Publish<CancelBuildPathEvent>(EventPublishMode::Queued);
		}

		ImGui::End();
	}
}

