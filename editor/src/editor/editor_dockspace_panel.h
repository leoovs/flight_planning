#pragma once

#include "editor/editor_panel.h"

namespace editor
{
	class EditorDockspacePanel final : public EditorPanel
	{
	public:
		~EditorDockspacePanel() override = default;

		EditorPanelKind GetKind() const override;

		void OnImGui() override;
	};
}

