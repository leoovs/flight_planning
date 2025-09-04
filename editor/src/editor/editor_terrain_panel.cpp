#include "editor/editor_terrain_panel.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "editor/editor_panel.h"
#include "event/event_bus.h"
#include "event/event_publisher.h"
#include "graphics/graphics_context.h"
#include "graphics/imgui_graphics_backend.h"
#include "graphics/subresource.h"
#include "graphics/texture_2d.h"
#include "runtime/rt_module_locator.h"

namespace editor
{
	EditorTerrainPanel::EditorTerrainPanel(
		TerrainEditor& terrainEditor,
		TerrainRenderer& renderer)
		: mTerrainEditor(&terrainEditor)
		, mRenderer(renderer)
	{}

	EditorPanelKind EditorTerrainPanel::GetKind() const
	{
		return EditorPanelKind::Terrain;
	}

	void EditorTerrainPanel::Connect(EventBus events)
	{
		mPublisher = EventPublisher(events);
		mSubscriber = EventSubscriber(events);

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorTerrainPanel::OnHeightMapLoaded)
			.EndClass();
	}

	void EditorTerrainPanel::OnImGui()
	{
		auto imguiGraphics = RtModuleLocator::Locate<ImGuiGraphicsBackend>();

		if (!mHeightMapTexture)
		{
			return;
		}

		ImGui::Begin("Terrain");

		mPanelSize = ImGui::GetWindowSize();

		ImTextureID heightMapID = (ImTextureID)imguiGraphics->GetTextureID(mHeightMapTexture.get());
		ImGui::Text("Height map");
		ImGui::Indent(10.0f);
		ImGui::Image(heightMapID, FitHeightMapTextureSize(), { 0, 1 }, { 1, 0 });
		ImGui::Indent(-10.0f);

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Display Mode");
		ImGui::Indent(10.0f);
		auto displayMode = static_cast<int>(mRenderer.GetDisplayMode());
		ImGui::Combo(
			"##",
			&displayMode,
			[](void* userptr, int item) -> const char*
			{
				return ToString(static_cast<TerrainDisplayMode>(item)).data();
			},
			nullptr,
			2);
		mRenderer.SetDisplayMode(static_cast<TerrainDisplayMode>(displayMode));	
		ImGui::Indent(-10.0f);

		glm::vec2 heightMapResolution = mTerrainEditor->GetHeightMapResolution();
		float worldScale = mTerrainEditor->GetWorldScale();
		float heightScale = mTerrainEditor->GetHeightScale();
		bool scaleUpdated = false;

		ImGui::Text("World scale");
		ImGui::Indent(10.0f);
		scaleUpdated = ImGui::DragFloat("##WORLD-SCALE", &worldScale, 0.1f, 0.0f, 0.0f, "%.1f")
			|| scaleUpdated;
		ImGui::Indent(-10.0f);

		ImGui::Text("Height scale");
		ImGui::Indent(10.0f);
		scaleUpdated = ImGui::DragFloat("##HEIGHT-SCALE", &heightScale, 0.1f, 0.0f, 0.0f, "%.1f")
			|| scaleUpdated;
		ImGui::Indent(-10.0f);

		if (scaleUpdated)
		{
			mPublisher.Publish<UpdateTerrainScaleEvent>(
				EventPublishMode::Queued,
				worldScale,
				heightScale);
		}

		ImGui::End();
	}

	bool EditorTerrainPanel::OnHeightMapLoaded(const HeightMapLoadedEvent& event)
	{
		UpdateHeightMapTexture();
		return true;
	}

	void EditorTerrainPanel::UpdateHeightMapTexture()
	{
		const uavpf::TiffImage& image = mTerrainEditor->GetHeightMapImage();

		Texture2DParams hmTextureParams;
		hmTextureParams.DebugName = "HM-VIZ";
		hmTextureParams.Width = image.GetWidth();
		hmTextureParams.Height = image.GetHeight();
		hmTextureParams.MipLevelCount = 1;
		hmTextureParams.Format = GraphicsFormat::R8G8B8A8_UNORM;

		auto graphics = RtModuleLocator::Locate<GraphicsContext>();

		mHeightMapTexture.reset(
			graphics->CreateTexture2D(std::move(hmTextureParams)),
			[graphics](Texture2D* texture)
			{
				graphics->DestroyTexture2D(texture);
			}
		);

		SubresourceRegion region;
		region.Width = image.GetWidth();
		region.Height = image.GetHeight();

		mHeightMapTexture->SetData(region, image.GetPixels());
	}

	ImVec2 EditorTerrainPanel::FitHeightMapTextureSize()
	{
		const uavpf::TiffImage& image = mTerrainEditor->GetHeightMapImage();

		ImVec2 imageSize
		{
			static_cast<float>(image.GetWidth()),
			static_cast<float>(image.GetHeight()),
		};

		float ratio = imageSize.x / imageSize.y;

		ImVec2 reducedSize
		{
			mPanelSize.x * 0.75f,
			mPanelSize.x * 0.75f / ratio,
		};

		float minWidth = 100.0f;
		float maxWidth = 500.0f;

		ImVec2 clapmedSize
		{
			glm::clamp(reducedSize.x, minWidth, maxWidth),
			glm::clamp(reducedSize.y, minWidth / ratio, maxWidth / ratio),
		};

		return clapmedSize;
	}
}

