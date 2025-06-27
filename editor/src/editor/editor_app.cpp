#include "editor/editor_app.h"

#include "editor/editor_debug_panel.h"
#include "editor/editor_dockspace_panel.h"
#include "editor/editor_terrain_panel.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "runtime/main_loop.h"

namespace editor
{
	void EditorApp::Setup()
	{
		RegisterPanel<EditorDockspacePanel>();
		RegisterPanel<EditorDebugPanel>();
		RegisterPanel<EditorTerrainPanel>();

		Enable(EditorPanelKind::Dockspace);
		Enable(EditorPanelKind::Terrain);
	}

	void EditorApp::Connect(EventBus events)
	{
		mEvents = events;
		mPublisher = EventPublisher(events);
		mSubscriber = EventSubscriber(events);

		for (const std::unique_ptr<EditorPanel>& panel : mPanels)
		{
			if (panel)
			{
				panel->Connect(events);
			}
		}

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorApp::OnWindowClosed)
			.EndClass();
	}

	void EditorApp::Update(float dt)
	{
		ForEachEnabledPanel(
			[dt](EditorPanel& panel)
			{
				panel.Update(dt);
			}
		);
	}

	void EditorApp::OnImGui()
	{
		ForEachEnabledPanel(
			[](EditorPanel& panel)
			{
				panel.OnImGui();
			}
		);
	}

	void EditorApp::Render()
	{
		ForEachEnabledPanel(
			[](EditorPanel& panel)
			{
				panel.Render();
			}
		);
	}

	void EditorApp::Enable(EditorPanelKind kind)
	{
		std::unique_ptr<EditorPanel>& panel = mPanels.at(+kind);
		if (nullptr == panel)
		{
			return;
		}
		
		bool& isEnabled = mIsEnabled.at(+kind);
		if (!isEnabled)
		{
			panel->OnEnable();
			isEnabled = true;
		}
	}

	void EditorApp::Disable(EditorPanelKind kind)
	{
		std::unique_ptr<EditorPanel>& panel = mPanels.at(+kind);
		if (nullptr == panel)
		{
			return;
		}

		bool& isEnabled = mIsEnabled.at(+kind);
		if (isEnabled)
		{
			panel->OnDisable();
			isEnabled = false;
		}
	}

	bool EditorApp::IsEnabled(EditorPanelKind kind) const
	{
		return mPanels.at(+kind) && mIsEnabled.at(+kind);
	}

	bool EditorApp::OnWindowClosed(const WindowCloseEvent& event)
	{
		mPublisher.Publish<MainLoopQuitEvent>(EventPublishMode::Queued);
		return true;
	}

	void EditorApp::ForEachEnabledPanel(std::function<void(EditorPanel&)> iteration)
	{
		for (const std::unique_ptr<EditorPanel>& panel : mPanels)
		{
			if (panel && mIsEnabled.at(+panel->GetKind()))
			{
				iteration(*panel);
			}
		}
	}
}

