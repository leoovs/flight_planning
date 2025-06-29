#include "editor/editor_menu_bar.h"

#include <imgui/imgui.h>
#include <tinyfiledialogs.h>

#include "editor/editor_events.h"

namespace editor
{
	EditorPanelKind EditorMenuBar::GetKind() const
	{
		return EditorPanelKind::MenuBar;
	}

	void EditorMenuBar::Connect(EventBus events)
	{
		mEvents = events;

		mPublisher = EventPublisher(events);
	}

	void EditorMenuBar::OnImGui()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Import"))
				{
					if (ImGui::MenuItem("TIFF Height Map"))
					{
						const char* tiffFilters[]{ "*.tif", "*.tiff" };
						const char* filePath = tinyfd_openFileDialog(
								"TIFF map",
								nullptr,
								std::size(tiffFilters),
								tiffFilters,
								nullptr,
								0);
						if (nullptr != filePath)
						{
							RequestLoadHeightMap(filePath);
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Close"))
				{
					mPublisher.Publish<CloseHeightMapRequestEvent>(EventPublishMode::Queued);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	void EditorMenuBar::Update(float dt)
	{
	}

	void EditorMenuBar::RequestLoadHeightMap(const char* filePath)
	{
		std::filesystem::path assetPath(filePath);	
		mPublisher.Publish<HeightMapRequestedEvent>(EventPublishMode::Queued, std::move(assetPath));
	}
}

