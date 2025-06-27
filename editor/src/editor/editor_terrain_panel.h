#pragma once

#include "editor/editor_panel.h"

namespace editor
{
	class EditorTerrainPanel final : public EditorPanel
	{
	public:
		~EditorTerrainPanel() override = default;

		EditorPanelKind GetKind() const override;

		void OnImGui() override;
	};
}

