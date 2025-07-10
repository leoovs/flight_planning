#include "editor/editor_scene_panel.h"

#include <imgui/imgui.h>

#include "editor/path_planner.h"
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
		const TerrainEditor& terrainEditor,
		const PathPlanner& pathPlanner,
		const NavNetwork& navNetwork)
		: mGraphics(graphics)
		, mImGuiGraphics(imguiGraphics)
		, mTerrainEditor(&terrainEditor)
		, mPathPlanner(&pathPlanner)
		, mNavNetwork(&navNetwork)
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
				.SubscribeMethod(&EditorScenePanel::OnUpdateNavGridResolution)
				.SubscribeMethod(&EditorScenePanel::OnUpdateCheckpointNavCoord)
				.SubscribeMethod(&EditorScenePanel::OnPathBuilt)
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

		if (glm::length(cameraMovement) && mIsFocused)
		{
			cameraMovement = glm::normalize(cameraMovement);
			mFreeCamera.SetPosition(mFreeCamera.GetPosition() + cameraMovement * dt);
		}

		mModelMatrix = mTerrainEditor->GetHeightMapToWorldMatrix();
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
		RenderCheckpoints();
		RenderPath();
		RenderNotams();

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
				0.05f,
				50.0f));

		auto image = (ImTextureID)mImGuiGraphics->GetTextureID(mColorBuffer.get());
		ImGui::Image(image, mPanelSize, { 0, 1 }, { 1, 0 });

		ImGui::EndChild();
		ImGui::End();
	}

	void EditorScenePanel::CreateRenderTextures(int32_t width, int32_t height)
	{
		mTerrainRenderer.SetTargetTextures(nullptr, nullptr);
		mTerrainRenderer.SetTargetTextures(nullptr, nullptr);

		Texture2DParams params;
		params.DebugName = "SCENE-CLR";
		params.Format = GraphicsFormat::R8G8B8A8_UNORM;
		params.Width = std::max(width, 8);
		params.Height = std::max(height, 8);

		auto deleter = [this](Texture2D* texture)
		{
			mGraphics->DestroyTexture2D(texture);
		};

		mColorBuffer.reset(mGraphics->CreateTexture2D(params), deleter);
		params.DebugName = "SCENE-DPTH";
		params.Format = GraphicsFormat::D24_UNORM_S8_UINT;
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
		mTerrainRenderer.Render(mTerrainRenderMesh.get(), mModelMatrix);
	}

	void EditorScenePanel::RenderCoordinateAxes()
	{
		glm::vec3 front(0.0f, 0.0f, 1.0f);
		glm::vec3 right(1.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		glm::vec3 milkyRed(1.0f, 0.7f, 0.73f);
		glm::vec3 milkyGreen(0.7f, 1.0f, 0.76f);
		glm::vec3 milkyBlue(0.7f, 0.85f, 1.0f);

		mOverlayRenderer.IgnoreDepth(true);
		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), front, milkyRed);
		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), right, milkyGreen);
		mOverlayRenderer.RenderLine3D(glm::vec3(0.0f), up, milkyBlue);
	}

	void EditorScenePanel::RenderCheckpoints()
	{
		mOverlayRenderer.IgnoreDepth(false);
		for (ptrdiff_t iCheckpoint = 0; iCheckpoint < +CheckpointKind::Count_; iCheckpoint++)
		{
			glm::vec4 color = iCheckpoint
				? glm::vec4(0.8f, 0.2f, 0.2f, 1.0f)
				: glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);

			auto kind = CheckpointKind(iCheckpoint);
			glm::ivec2 navCoord = mPathPlanner->GetNavCoord(kind);
			glm::ivec2 hmCoord = mPathPlanner->NavCoordToHeightMapCoord(navCoord);
			glm::vec3 worldCoord = mTerrainEditor->HeightMapToWorldCoord(hmCoord);

			float radius = 0.01f;
			worldCoord.y += radius + mPathPlanner->GetWorldSpaceElevation();

			mOverlayRenderer.RenderCircle3D(worldCoord, radius, color);
		}
	}

	void EditorScenePanel::RenderPath()
	{
		mOverlayRenderer.IgnoreDepth(false);

		ptrdiff_t iCell = 1;
		for (; iCell < mCachedPath.size(); iCell++)
		{
			ptrdiff_t iPrevCell = iCell - 1;

			glm::ivec2 navCoord = mCachedPath.at(iCell).NavCoords;
			glm::ivec2 prevNavCoord = mCachedPath.at(iPrevCell).NavCoords;

			glm::ivec2 hmCoord = mPathPlanner->NavCoordToHeightMapCoord(navCoord);
			glm::ivec2 prevHmCoord = mPathPlanner->NavCoordToHeightMapCoord(prevNavCoord);

			glm::vec3 worldCoord = mTerrainEditor->HeightMapToWorldCoord(hmCoord);
			glm::vec3 prevWorldCoord = mTerrainEditor->HeightMapToWorldCoord(prevHmCoord);

			float radius = 0.003f;
			worldCoord.y += 0.01f + mPathPlanner->GetWorldSpaceElevation();
			prevWorldCoord.y += 0.01f + mPathPlanner->GetWorldSpaceElevation();

			mOverlayRenderer.RenderLine3D(worldCoord, prevWorldCoord, glm::vec3(1.0f));
			mOverlayRenderer.RenderCircle3D(worldCoord, radius, glm::vec3(1.0f));
		}
	}

	void EditorScenePanel::RenderNotams()
	{
		mOverlayRenderer.IgnoreDepth(false);

		size_t notamCount = mNavNetwork->GetNotamCount();
		for (size_t iNotam = 0; iNotam < notamCount; iNotam++)
		{
			Notam notam = mNavNetwork->GetNotam(iNotam);

			int32_t heightMapRadius = notam.RelativeRadius * mTerrainEditor->GetHeightMapResolution().x;
			float worldSpaceRadius = mTerrainEditor->HeightMapToWorldCoord({ heightMapRadius, 0 }).x;

			glm::vec3 worldSpaceCoord = mTerrainEditor->HeightMapToWorldCoord(
				mPathPlanner->NavCoordToHeightMapCoord(notam.NavCoord));
			worldSpaceCoord.y += 0.01f + mPathPlanner->GetWorldSpaceElevation();

			float circleWidth = 0.0f == notam.RelativeRadius
				? 0.01f
				: 0.005f;
			mOverlayRenderer.RenderCircle3D(worldSpaceCoord, circleWidth, glm::vec3(1.0f, 1.0f, 0.3f));

			int32_t segmentCount = 30;
			float deltaAngle = glm::two_pi<float>() / segmentCount;

			for (int32_t iSegment = 0; iSegment < segmentCount; iSegment++)
			{
				int32_t nextSegment = (iSegment + 1) % segmentCount;

				glm::vec3 offset{};
				offset.x = worldSpaceRadius * glm::cos(iSegment * deltaAngle);
				offset.z = worldSpaceRadius * glm::sin(iSegment * deltaAngle);

				glm::vec3 nextOffset{};
				nextOffset.x = worldSpaceRadius * glm::cos(nextSegment * deltaAngle);
				nextOffset.z = worldSpaceRadius * glm::sin(nextSegment * deltaAngle);

				mOverlayRenderer.RenderLine3D(
					worldSpaceCoord + offset,
					worldSpaceCoord + nextOffset,
					glm::vec3(1.0f, 1.0f, 0.3f));
			}
		}
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

	bool EditorScenePanel::OnUpdateNavGridResolution(const UpdateNavGridResolutionEvent& event)
	{
		mCachedPath.clear();
		return true;
	}

	bool EditorScenePanel::OnUpdateCheckpointNavCoord(const UpdateCheckpointNavCoordEvent& event)
	{
		mCachedPath.clear();
		return true;
	}

	bool EditorScenePanel::OnPathBuilt(const PathBuiltEvent& event)
	{
		mCachedPath = mPathPlanner->GetPath();
		return true;
	}
}

