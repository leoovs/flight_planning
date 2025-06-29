#pragma once

#include "editor/editor_panel.h"
#include "editor/path_planner.h"
#include "event/event_publisher.h"

namespace editor
{
	class EditorNavPanel final : public EditorPanel
	{
	public:
		EditorNavPanel(PathPlanner& pathPlanner);
		~EditorNavPanel() override = default;

		EditorPanelKind GetKind() const override;

		void Connect(EventBus events) override;

		void OnImGui() override;

	private:
		EventPublisher mPublisher;

		PathPlanner* mPathPlanner = nullptr;
	};
}

