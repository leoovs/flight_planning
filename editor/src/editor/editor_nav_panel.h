#pragma once

#include "editor/editor_panel.h"
#include "editor/nav_network.h"
#include "editor/path_planner.h"
#include "event/event_publisher.h"

namespace editor
{
	class EditorNavPanel final : public EditorPanel
	{
	public:
		EditorNavPanel(const PathPlanner& pathPlanner, const NavNetwork& navNetwork);
		~EditorNavPanel() override = default;

		EditorPanelKind GetKind() const override;

		void Connect(EventBus events) override;

		void OnImGui() override;

	private:
		EventPublisher mPublisher;

		const PathPlanner* mPathPlanner = nullptr;
		const NavNetwork* mNavNetwork = nullptr;
	};
}

