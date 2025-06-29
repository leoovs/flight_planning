#include "editor/editor_scene_panel.h"

#include <imgui/imgui.h>

#include "editor/terrain_editor.h"
#include "imgui_internal.h"
#include "platform/keyboard.h"
#include "platform/mouse.h"
#include "runtime/rt_module_locator.h"
#include "tasking/task_scheduler.h"

namespace editor
{
	EditorScenePanel::EditorScenePanel(
		GraphicsContext* graphics,
		ImGuiGraphicsBackend* imguiGraphics,
		TerrainEditor& terrainEditor)
		: mGraphics(graphics)
		, mImGuiGraphics(imguiGraphics)
		, mTerrainEditor(&terrainEditor)
		, mTerrainRenderer(graphics)
		, mOverlayRenderer(graphics)
	{
		CreateRenderTextures(8, 8);
	}

	EditorPanelKind EditorScenePanel::GetKind() const
	{
		return EditorPanelKind::Scene;
	}

	void EditorScenePanel::Connect(EventBus events)
	{
		mSubscriber = EventSubscriber(events);

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&EditorScenePanel::OnHeightMapRequested)
				.SubscribeMethod(&EditorScenePanel::OnHeightMapLoaded)
				.SubscribeMethod(&EditorScenePanel::OnMouseMovement)
			.EndClass();
	}

	void EditorScenePanel::Update(float dt)
	{
		auto* keyboard = RtModuleLocator::Locate<Keyboard>();
		glm::vec3 cameraMovement(0.0f);

		if (keyboard->IsKeyDown(Key::W))
		{
			cameraMovement += mFreeCamera.GetFrontVector();
		}
		if (keyboard->IsKeyDown(Key::A))
		{
			cameraMovement -= mFreeCamera.GetRightVector();
		}
		if (keyboard->IsKeyDown(Key::S))
		{
			cameraMovement -= mFreeCamera.GetFrontVector();
		}
		if (keyboard->IsKeyDown(Key::D))
		{
			cameraMovement += mFreeCamera.GetRightVector();
		}

		if (glm::length(cameraMovement))
		{
			cameraMovement = glm::normalize(cameraMovement);
			mFreeCamera.SetPosition(mFreeCamera.GetPosition() + cameraMovement * dt);
		}
	}

	void EditorScenePanel::Render()
	{
		mTerrainRenderer.Clear();

		if (!mTerrainRenderMesh)
		{
			return;
		}

		Viewport vp{ 0, 0, static_cast<int32_t>(mPanelSize.x), static_cast<int32_t>(mPanelSize.y) };
		mGraphics->SetViewport(vp);

		mTerrainRenderer.SetCamera(mFreeCamera);
		mOverlayRenderer.SetCamera(mFreeCamera);

		RenderTerrain();
		RenderCoordinateAxes();

		mGraphics->SetFramebuffer(nullptr);
	}

	void EditorScenePanel::OnImGui()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");
		ImGui::PopStyleVar();
		ImGui::BeginChild("SCENE-RENDER", { 0, 0 }, 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

		mIsFocused = ImGui::IsWindowFocused();
		ImVec2 currentPanelSize = ImGui::GetWindowSize();
		if (currentPanelSize.x != mPanelSize.x ||
			currentPanelSize.y != mPanelSize.y)
		{
			CreateRenderTextures(currentPanelSize.x, currentPanelSize.y);
		}
		mPanelSize = currentPanelSize;
		mFreeCamera.SetProjectionMatrix(
			glm::perspective(
				glm::radians(50.0f),
				mPanelSize.x / mPanelSize.y,
				0.1f,
				100.0f));

		auto image = (ImTextureID)mImGuiGraphics->GetTextureID(mColorBuffer.get());
		ImGui::Image(image, mPanelSize, { 0, 1 }, { 1, 0 });

		ImGui::EndChild();
		ImGui::End();
	}

	void EditorScenePanel::CreateRenderTextures(int32_t width, int32_t height)
	{
		Texture2DParams params;
		params.DebugName = "SCENE-CLR";
		params.Format = GraphicsFormat::R8G8B8A8_UNORM;
		params.Width = width;
		params.Height = height;

		auto deleter = [this](Texture2D* texture)
		{
			mGraphics->DestroyTexture2D(texture);
		};

		mColorBuffer.reset(mGraphics->CreateTexture2D(params), deleter);
		params.Format = GraphicsFormat::D32_FLOAT;
		mDepthBuffer.reset(mGraphics->CreateTexture2D(params), deleter);

		mTerrainRenderer.SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mOverlayRenderer.SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
	}

	void EditorScenePanel::UpdateTerrainMesh()
	{
		auto tasks = RtModuleLocator::Locate<TaskScheduler>();

		auto buildTerrainMesh = [this]()
		{
			const uavpf::HeightMap& hm = mTerrainEditor->GetHeightMap();
			mTerrainMesh = uavpf::TerrainMeshBuilder()
				.SetHeight(hm)
				.Build();
		};

		auto uploadTerrainMesh = [this]()
		{
			mTerrainRenderMesh = std::make_unique<TerrainRenderMesh>(
				mGraphics,
				mTerrainMesh);
		};

		auto task = TaskBuilder()
			.BeginSequence()
				.DoThreaded(buildTerrainMesh)
				.Do(uploadTerrainMesh)
			.End()
			.Build();

		tasks->Push(std::move(task));
	}

	void EditorScenePanel::PlaceCamera()
	{
		mFreeCamera.SetPosition({ 1.3f, 2.0f, 1.3f });
		mFreeCamera.LookAt({ 0.5f, 0.0f, 0.5f });
	}

	void EditorScenePanel::RenderTerrain()
	{
		glm::mat4 modelMatrix = mTerrainEditor->GetTerrainScaleAsMatrix();
		mTerrainRenderer.Render(mTerrainRenderMesh.get(), modelMatrix);
	}

	void EditorScenePanel::RenderCoordinateAxes()
	{
		glm::vec3 front(0.0f, 0.0f, 1.0f);
		glm::vec3 right(1.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), front, front);
		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), right, right);
		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), up, up);
	}

	bool EditorScenePanel::OnHeightMapRequested(const HeightMapRequestedEvent& event)
	{
		mTerrainRenderMesh.reset();
		return true;
	}

	bool EditorScenePanel::OnHeightMapLoaded(const HeightMapLoadedEvent& event)
	{
		UpdateTerrainMesh();
		PlaceCamera();

		return true;
	}

	bool EditorScenePanel::OnMouseMovement(const MouseMovementEvent& event)
	{
		auto* mouse = RtModuleLocator::Locate<Mouse>();

		if (!mIsFocused || !mouse->IsButtonPressed(MouseButton::Right))
		{
			return true;
		}

		float deltaPolar = event.DeltaY * 0.01f;
		float deltaAzimuth = event.DeltaX * 0.01f;

		auto [polar, azimuth] = mFreeCamera.CalculatePolarAndAzimuth();
		polar = glm::clamp(polar + deltaPolar,  0.01f, glm::pi<float>() - 0.1f);
		azimuth += deltaAzimuth;

		mFreeCamera.LookAround(polar, azimuth);

		return true;
	}
}

