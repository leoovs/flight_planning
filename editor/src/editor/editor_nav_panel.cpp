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
		ImGui::Indent(10.0f);
		if (ImGui::DragInt2("##NAV-RES", &res.Width, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			mPublisher.Publish<UpdateNavGridResolutionEvent>(
				EventPublishMode::Immediate,
				res);
		}
		ImGui::Indent(-10.0f);

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Nav coordinates");
		ImGui::Indent(10.0f);
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
				navCoordUpdated = ImGui::DragInt("##NAV-COORD-WIDTH", &navCoord.x, 1, 0, res.Width-1, "%d", ImGuiSliderFlags_AlwaysClamp)
					|| navCoordUpdated;
				ImGui::TableSetColumnIndex(2);
				navCoordUpdated = ImGui::DragInt("##NAV-COORD-DEPTH", &navCoord.y, 1, 0, res.Depth-1, "%d", ImGuiSliderFlags_AlwaysClamp)
					|| navCoordUpdated;

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
		ImGui::Indent(-10.0f);

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

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Weights");
		ImGui::Indent(10.0f);
		ImGui::BeginTable("PATH-PLANNER-WEIGHTS", 2);
		for (size_t iWeight = 0; iWeight < +PathPlannerWeight::Count_; iWeight++)
		{
			ImGui::PushID(iWeight);

			ImGui::TableNextRow();

			auto name = PathPlannerWeight(iWeight);
			float weight = mPathPlanner->GetWeight(name);

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", ToString(name).data());
			ImGui::TableSetColumnIndex(1);
			if (ImGui::DragFloat("##DRAG-PATH-PLANNER-WEIGHT", &weight, 0.1f, 0.0f, 0.0f, "%.1f"))
			{
				mPublisher.Publish<UpdatePathPlannerWeightEvent>(EventPublishMode::Queued, name, weight);
			}
			
			ImGui::PopID();
		}
		ImGui::Indent(-10.0f);
		ImGui::EndTable();

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Notams");
		ImGui::SameLine();
		float buttonWidth = ImGui::GetFrameHeight();
		if (ImGui::Button("+", ImVec2(buttonWidth, buttonWidth)))
		{
			mPublisher.Publish<AddNotamEvent>(EventPublishMode::Queued);
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear all"))
		{
			mPublisher.Publish<ClearNotamsEvent>(EventPublishMode::Queued);
		}

		ImGui::BeginChild("###NOTAM-LIST", ImVec2(0, 200), ImGuiChildFlags_Border);
		size_t notamCount = mNavNetwork->GetNotamCount();
		for (size_t iNotam = 0; iNotam < notamCount; iNotam++)
		{
			ImGui::PushID(iNotam);
			if (ImGui::TreeNode(("#" + std::to_string(iNotam+1)).data()))
			{
				Notam notam = mNavNetwork->GetNotam(iNotam);
				bool updated = false;

				if (ImGui::BeginTable("##NOTAM-COORD_DRAG", 2))
				{
					ImGui::TableNextRow();
					
					ImGui::TableSetColumnIndex(0);
					updated = ImGui::DragInt("##NOTAM-COORD-WIDTH", &notam.NavCoord.x, 1.0f, 0, res.Width-1)
						|| updated;
					ImGui::TableSetColumnIndex(1);
					updated = ImGui::DragInt("##NOTAM-COORD-DEPTH", &notam.NavCoord.y, 1.0f, 0, res.Depth-1)
						|| updated;

					ImGui::EndTable();
				}
				updated = ImGui::DragFloat("Radius", &notam.RelativeRadius, 0.001f, 0.0f, 1.0f)
					|| updated;

				if (updated)
				{
					mPublisher.Publish<UpdateNotamEvent>(EventPublishMode::Queued, notam, iNotam);
				}

				if (ImGui::Button("Remove"))
				{
					mPublisher.Publish<RemoveNotamEvent>(EventPublishMode::Queued, iNotam);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		ImGui::EndChild();

		ImGui::End();
	}
}

