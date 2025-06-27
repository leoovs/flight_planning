#include "editor/editor_terrain_panel.h"

#include <imgui/imgui.h>

#include "editor/editor_panel.h"
#include "graphics/graphics_context.h"
#include "graphics/imgui_graphics_backend.h"
#include "graphics/subresource.h"
#include "graphics/texture_2d.h"
#include "runtime/rt_module_locator.h"

namespace editor
{
	EditorTerrainPanel::EditorTerrainPanel(TerrainEditor& terrainEditor)
		: mTerrainEditor(&terrainEditor)
	{}

	EditorPanelKind EditorTerrainPanel::GetKind() const
	{
		return EditorPanelKind::Terrain;
	}

	void EditorTerrainPanel::Connect(EventBus events)
	{
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

		if (ImGui::Begin("Terrain"))
		{
			mPanelSize = ImGui::GetWindowSize();

			ImTextureID heightMapID = (ImTextureID)imguiGraphics->GetTextureID(mHeightMapTexture.get());
			ImGui::Image(heightMapID, FitHeightMapTextureSize(), { 0, 1 }, { 1, 0 });
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
		float maxWidth = 300.0f;

		ImVec2 clapmedSize
		{
			glm::clamp(reducedSize.x, minWidth, maxWidth),
			glm::clamp(reducedSize.y, minWidth / ratio, maxWidth / ratio),
		};

		return clapmedSize;
	}
}

