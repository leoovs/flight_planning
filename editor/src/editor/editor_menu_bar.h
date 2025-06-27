#pragma once

#include "event/event_bus.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "editor/editor_panel.h"

namespace editor
{
	class EditorMenuBar final : public EditorPanel
	{
	public:
		~EditorMenuBar() override = default;

		EditorPanelKind GetKind() const override;

		void Connect(EventBus events) override;

		void OnImGui() override;
		void Update(float dt) override;

	private:
		void RequestLoadHeightMap();

		EventBus mEvents;
		EventPublisher mPublisher;
		const char* mHeightMapFilePath = nullptr;
	};
}

