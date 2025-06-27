#pragma once

#include "editor/editor_panel.h"

namespace editor
{
	class EditorDebugPanel final : public EditorPanel
	{
	public:
		~EditorDebugPanel() override = default;

		EditorPanelKind GetKind() const override;

		void OnImGui() override;
	};
}

