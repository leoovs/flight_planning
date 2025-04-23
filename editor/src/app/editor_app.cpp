#include "app/editor_app.h"

#include "app/app_events.h"
#include "event/event_subscriber.h"
#include "imgui_internal.h"

#include <uavpf/uavpf.h>
#include <imgui.h>

namespace editor
{
	class PathFoundEvent : public Event
	{
	public:
		std::vector<uavpf::NavNode*> Path;

		PathFoundEvent(std::vector<uavpf::NavNode*> path)
			: Path(std::move(path))
		{
		}
	};

	void EditorApp::RegisterService(AppService* service)
	{
		mService = service;
	}

	void EditorApp::Bind(EventBus& bus)
	{
		mEventSubscriber = EventSubscriber(bus);
		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClose)
				.SubscribeMethod(&EditorApp::OnPathFound)
			.EndClass();

		mEventPublisher = EventPublisher(bus);
	}

	void EditorApp::OnUI()
	{
		std::string name = "";
		switch (mState)
		{
			case EditorState::WaitingOnLoad:
				name = "In progress";
				break;
			case EditorState::Operating:
				name = "Operating";
				break;
		}

		ImGui::Begin((name + "###Operation").data());

		bool start = false;
		if (mState == EditorState::WaitingOnLoad)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button("Start");
			ImGui::PopStyleVar();
			ImGui::PopItemFlag();
		}
		else
		{
			start = ImGui::Button("Start");
		}

		if (start)
		{
			mState = EditorState::WaitingOnLoad;
			mService->AddTask(
				[this]()
				{
					uavpf::TiffImage image = uavpf::TiffLoader()
						.LoadImageFromFile("C:/Users/Leonid/Desktop/mountain.tif");
					uavpf::HeightMap hm = uavpf::HeightMapBuilder()
						.SetGrayscale(uavpf::ImageGrayscale(image))
						.SetRasterSpace(uavpf::RasterSpace::RasterIsPoint)
						.Build();
					uavpf::TerrainMesh mesh = uavpf::TerrainMeshBuilder()
						.SetHeight(hm)
						.Build();

					uavpf::NavGridSpecification spec;
					spec.Width = 50;
					spec.Depth = 50;

					uavpf::NavGrid ng(spec);
					ng.SetHeightMap(&hm);

					uavpf::ElevationConservingCost cost(0.45f);
					uavpf::AStarAlgorithm pathFinder(&cost, &ng, { 0, 0 }, { 0.7 * 50, 0.5 * 50 });

					while (pathFinder.IsExplorable())
					{
						pathFinder.ExploreNext();
						if (pathFinder.IsGoal())
						{
							break;
						}

						pathFinder.ExploreNeighbour({ 1, 0, });
						pathFinder.ExploreNeighbour({ 0, 1, });
						pathFinder.ExploreNeighbour({ 0, -1, });
						pathFinder.ExploreNeighbour({ -1, 0, });
					}

					mState = EditorState::Operating;
				}
			);
		}
		ImGui::End();
	}

	void EditorApp::Render()
	{
		mService->GetGraphics()->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
	}

	bool EditorApp::OnWindowClose(const WindowCloseEvent& event)
	{
		mEventPublisher.Publish<AppQuitEvent>(EventPublishMode::Queued);	
		return true;
	}

	bool EditorApp::OnPathFound(const PathFoundEvent& event)
	{
		mPath = event.Path;
		mState = EditorState::Operating;
		return true;
	}
}

