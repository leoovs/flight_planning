#pragma once

#include <array>

#include "editor/editor_panel.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "platform/platform_events.h"
#include "runtime/app.h"

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

		template<typename PanelT>
		void RegisterPanel()
		{
			auto panel = std::make_unique<PanelT>();
			mPanels.at(+panel->GetKind()) = std::move(panel);
		}

		void ForEachEnabledPanel(std::function<void(EditorPanel&)> iteration);

		EventBus mEvents;
		EventPublisher mPublisher;
		EventSubscriber mSubscriber;

		std::array<std::unique_ptr<EditorPanel>, +EditorPanelKind::Count_>
		mPanels;
		std::array<bool, +EditorPanelKind::Count_> mIsEnabled;
	};
}

