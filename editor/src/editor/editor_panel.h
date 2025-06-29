#pragma once

#include <cstdint>

#include "event/event_bus.h"

namespace editor
{
	enum class EditorPanelKind
	{
		Dockspace,
		Debug,
		MenuBar,
		Terrain,
		Scene,
		Count_,
	};

	constexpr size_t operator+(EditorPanelKind kind)
	{
		return static_cast<size_t>(kind);
	}

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual EditorPanelKind GetKind() const = 0;

		virtual void Connect(EventBus events) {}

		virtual void OnEnable() {}
		virtual void OnDisable() {}

		virtual void Update(float dt) {}
		virtual void OnImGui() {}
		virtual void Render() {}
	};
}

