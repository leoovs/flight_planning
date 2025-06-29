#pragma once

#include <memory>

#include <imgui/imgui.h>

#include "editor/editor_panel.h"
#include "editor/editor_events.h"
#include "editor/terrain_editor.h"
#include "event/event_subscriber.h"
#include "graphics/imgui_graphics_backend.h"
#include "platform/platform_events.h"
#include "rendering/camera.h"
#include "rendering/overlay_renderer.h"
#include "rendering/terrain_renderer.h"
#include "rendering/terrain_render_mesh.h"

namespace editor
{
	class EditorScenePanel final : public EditorPanel
	{
	public:
		EditorScenePanel(
			GraphicsContext* graphics,
			ImGuiGraphicsBackend* imguiGraphics,
			TerrainEditor& terrainEditor);
		~EditorScenePanel() override = default;

		EditorPanelKind GetKind() const override;

		void Connect(EventBus events) override;

		void Update(float dt) override;
		void Render() override;
		void OnImGui() override;

	private:
		void CreateRenderTextures(int32_t width, int32_t height);
		void UpdateTerrainMesh();
		void PlaceCamera();

		void RenderTerrain();
		void RenderCoordinateAxes();

		bool OnHeightMapRequested(const HeightMapRequestedEvent& event);
		bool OnHeightMapLoaded(const HeightMapLoadedEvent& event);
		bool OnMouseMovement(const MouseMovementEvent& event);

		EventSubscriber mSubscriber;

		GraphicsContext* mGraphics = nullptr;
		ImGuiGraphicsBackend* mImGuiGraphics = nullptr;
		TerrainEditor* mTerrainEditor = nullptr;
		TerrainRenderer mTerrainRenderer;
		OverlayRenderer mOverlayRenderer;
		Camera mFreeCamera;

		ImVec2 mPanelSize{};
		bool mIsFocused = false;
		uavpf::TerrainMesh mTerrainMesh;
		std::unique_ptr<TerrainRenderMesh> mTerrainRenderMesh;
		std::shared_ptr<Texture2D> mColorBuffer;
		std::shared_ptr<Texture2D> mDepthBuffer;
	};
}

