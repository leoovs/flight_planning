#pragma once

#include <array>

#include "editor/editor_events.h"
#include "editor/editor_panel.h"
#include "editor/nav_network.h"
#include "editor/path_planner.h"
#include "editor/terrain_editor.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "platform/platform_events.h"
#include "runtime/app.h"
#include "runtime/rt_module_locator.h"
#include "tasking/task_scheduler.h"

namespace editor
{
	class EditorApp final : public App	
	{
	public:
		~EditorApp() override = default;

		void Setup() override;
		void Connect(EventBus events) override;

		void Update(float dt) override;
		void OnImGui() override;
		void Render() override;

		void Enable(EditorPanelKind kind);
		void Disable(EditorPanelKind kind);
		bool IsEnabled(EditorPanelKind kind) const;

	private:
		bool OnWindowClosed(const WindowCloseEvent& event);
		bool OnHeightMapRequested(const HeightMapRequestedEvent& event);
		bool OnCloseHeightMapRequested(const CloseHeightMapRequestEvent& event);
		bool OnUpdateNavGridResolutionEvent(const UpdateNavGridResolutionEvent& event);
		bool OnUpdateCheckpointNavCoord(const UpdateCheckpointNavCoordEvent& event);
		bool OnBuildPath(const BuildPathEvent& event);
		bool OnCancelBuildPath(const CancelBuildPathEvent& event);

		template<typename PanelT, typename... ArgsT>
		void RegisterPanel(ArgsT&&... args)
		{
			auto panel = std::make_unique<PanelT>(std::forward<ArgsT>(args)...);
			mPanels.at(+panel->GetKind()) = std::move(panel);
		}

		void ForEachEnabledPanel(std::function<void(EditorPanel&)> iteration);

		EventBus mEvents;
		EventPublisher mPublisher;
		EventSubscriber mSubscriber;

		std::array<std::unique_ptr<EditorPanel>, +EditorPanelKind::Count_>
		mPanels;
		std::array<bool, +EditorPanelKind::Count_> mIsEnabled;

		TaskScheduler* mTasks = nullptr;

		TerrainEditor mTerrainEditor;
		NavNetwork mNavNetwork{ mTerrainEditor };
		PathPlanner mPathPlanner{ mTerrainEditor, mNavNetwork };
	};
}

