#pragma once

#include <imgui/imgui.h>

#include "event/event_bus.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "editor/editor_panel.h"
#include "editor/editor_events.h"
#include "editor/terrain_editor.h"
#include "graphics/texture_2d.h"

namespace editor
{
	class EditorTerrainPanel final : public EditorPanel
	{
	public:
		EditorTerrainPanel(TerrainEditor& terrainEditor);
		~EditorTerrainPanel() override = default;

		void Connect(EventBus events) override;

		EditorPanelKind GetKind() const override;

		void OnImGui() override;

	private:
		void ShowTerrainScaleControl();

		bool OnHeightMapLoaded(const HeightMapLoadedEvent& event);

		void UpdateHeightMapTexture();
		ImVec2 FitHeightMapTextureSize();

		EventSubscriber mSubscriber;
		EventPublisher mPublisher;
		TerrainEditor* mTerrainEditor = nullptr;
		std::shared_ptr<Texture2D> mHeightMapTexture;
		ImVec2 mPanelSize{};
	};
}

