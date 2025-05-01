#include "app/editor_state.h"

#include "app/editor_app.h"
#include "app/path_mission.h"
#include "imgui_internal.h"
#include "uavpf/algo/astar_cost.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// Editor_Idle
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	EditorStateKind Editor_Idle::GetKind() const
	{
		return EditorStateKind::Idle;
	}

	void Editor_Idle::OnAttach(EditorApp* app)
	{
		mApp = app;
		EventBus& events = mApp->GetEvents();

		mPublisher = EventPublisher(events);

		mSubscriber = EventSubscriber(events);
		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&Editor_Idle::OnTiffMapRequested)
			.EndClass();
	}

	void Editor_Idle::OnDetach()
	{
		mSubscriber = {};
	}

	void Editor_Idle::ShowMenu()
	{
		const char* tiffMapPath = nullptr;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open TIFF map"))
				{
					const char* tiffFilters[]{ "*.tif", "*.tiff" };
					tiffMapPath = tinyfd_openFileDialog(
						"TIFF map",
						nullptr,
						std::size(tiffFilters),
						tiffFilters,
						nullptr,
						0);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (tiffMapPath)
		{
			mPublisher.Publish<TiffMapRequestedEvent>(EventPublishMode::Queued, tiffMapPath);
		}
	}

	bool Editor_Idle::OnTiffMapRequested(const TiffMapRequestedEvent& event)
	{
		mApp->GetContext()->GetAssets().LoadAssetAsync(
			event.TiffMapPath,
			AssetKind::Image,
			[this](AssetID id)
			{
				if (uavpf::cBadID == id)
				{
					return;
				}

				mApp->GetContext()->SetMapImage(id);
				mApp->Push(EditorStateKind::PathBuilder);
			}
		);

		return true;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// Editor_PathBuilder
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	EditorStateKind Editor_PathBuilder::GetKind() const 
	{
		return EditorStateKind::PathBuilder;
	}

	void Editor_PathBuilder::OnAttach(EditorApp* app)
	{
		mApp = app;

		mSubscriber = EventSubscriber(mApp->GetEvents());
		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&Editor_PathBuilder::OnMouseMove)
			.EndClass();

		SetupRenderBuffers(720, 480);
		SetupRenderers();

		mFreeCamera.SetPosition(glm::vec3(0.0f));
		mFreeCamera.LookAt(glm::vec3(0.0f, 0.0f, 1.0f));

		mProgressTitle = "Initializing PathBuilder";
		mProgressChar = "\n*";
		mApp->GetService()->AddTask(
			[this]()
			{
				mState = State::Preparing;
				InitializePathMission();
				InitializeTerrainMesh();
				mState = State::Idle;
			}
		);
	}

	void Editor_PathBuilder::OnDetach()
	{
		mSubscriber = {};
	}

	void Editor_PathBuilder::Update()
	{
		switch (mState)
		{
			case State::Preparing:
				UpdateProgressMessage();
				break;	
			case State::Idle:
			case State::PathFinding:
				UpdateProgressMessage();
				HandleCameraInput();
				LoadTerrainMesh();
				break;
		}
	}

	void Editor_PathBuilder::Render()
	{
		if (mState == State::Preparing)
		{
			return;
		}

		if (nullptr == mTerrainRenderMesh)
		{
			return;
		}

		Viewport vp;
		vp.Width = mColorBuffer->GetParams().Width;
		vp.Height = mColorBuffer->GetParams().Height;
		mApp->GetService()->GetGraphics()->SetViewport(vp);

		mTerrainRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mTerrainRenderer->Clear();
		mTerrainRenderer->SetCamera(mFreeCamera);
		mTerrainRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mTerrainRenderer->Render(mTerrainRenderMesh.get(), mApp->GetContext()->GetTerrainScaler().GetModelMatrix());

		Render_PathMissionTargets();
		Render_Path();

		mOverlayRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mOverlayRenderer->SetCamera(mFreeCamera);
		mOverlayRenderer->IgnoreDepth(true);
		mOverlayRenderer->RenderLine3D(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mOverlayRenderer->RenderLine3D(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mOverlayRenderer->RenderLine3D(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		mApp->GetService()->GetGraphics()->SetFramebuffer(nullptr);
	}

	void Editor_PathBuilder::ShowMenu()
	{
		switch (mState)
		{
			case State::Preparing:
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.4f, 1.0f), "%s", mProgressMessage.data());
				break;
			case State::Idle:
			case State::PathFinding:
				Show_CloseMenu();
				break;
		}
	}

	void Editor_PathBuilder::ShowUI()
	{
		switch (mState)
		{
			case State::Preparing:
				break;
			case State::Idle:
			case State::PathFinding:
				Show_PathMissionPanel();
				Show_ScenePanel();
				Show_MapPanel();
				break;
		}
	}

	void Editor_PathBuilder::Render_PathMissionTargets()
	{
		PathMission& mission = mApp->GetContext()->GetPathMission();
		const uavpf::NavGrid& navGrid = mission.GetNavGrid();
		const uavpf::HeightMap* heightMap = navGrid.GetHeightMap();
		TerrainScaler& scaler = mApp->GetContext()->GetTerrainScaler();

		if (navGrid.GetSpecification().Width == 0 || navGrid.GetSpecification().Depth == 0)
		{
			return;
		}

		glm::ivec2 navStartPos = mission.GetStart();
		float startElevation = navGrid.GetElevation(navStartPos) + mission.GetMinElevation();
		glm::ivec2 navEndPos = mission.GetEnd();
		float endElevation = navGrid.GetElevation(navEndPos) + mission.GetMinElevation();

		glm::vec4 startPos  = NavCoordToWorldCoord(glm::vec3(navStartPos.x, startElevation, navStartPos.y));
		glm::vec4 endPos = NavCoordToWorldCoord(glm::vec3(navEndPos.x, endElevation, navEndPos.y));

		float radius = 0.03f;

		mOverlayRenderer->IgnoreDepth(false);
		mOverlayRenderer->RenderCircle3D(glm::vec3(startPos), radius, glm::vec3(0.0f, 1.0f, 0.0f));
		mOverlayRenderer->RenderCircle3D(glm::vec3(endPos), radius, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void Editor_PathBuilder::Render_Path()
	{
		if (mState != State::Idle)
		{
			return;
		}

		PathMission& mission = mApp->GetContext()->GetPathMission();
		if (mission.GetStatus() != PathMission::PathStatus::Found)
		{
			return;
		}

		TerrainScaler& scaler = mApp->GetContext()->GetTerrainScaler();
		const uavpf::NavGrid& navGrid = mission.GetNavGrid();

		mOverlayRenderer->IgnoreDepth(false);

		Path& path = mission.GetPath();
		const std::vector<glm::vec3>& navCoords = path.GetCoordinates();
		float minElevation = mission.GetMinElevation();

		ptrdiff_t first = 1;
		ptrdiff_t iCheckpoint = first;

		for (; iCheckpoint < path.GetCoordinates().size(); iCheckpoint++)
		{
			ptrdiff_t prev = iCheckpoint - 1;

			glm::vec3 navCoord = navCoords.at(iCheckpoint);
			glm::vec3 prevNavCoord = navCoords.at(prev);

			glm::vec4 coord = NavCoordToWorldCoord(navCoord);
			glm::vec4 prevCoord = NavCoordToWorldCoord(prevNavCoord);

			mOverlayRenderer->RenderLine3D(coord, prevCoord, glm::vec3(1.0f));
			mOverlayRenderer->RenderCircle3D(coord, 0.01f, glm::vec3(1.0f));
		}
	}

	void Editor_PathBuilder::Show_CloseMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close"))
				{
					mCanLoadMesh = false;
					mTerrainRenderMesh.reset();
					mApp->Push(EditorStateKind::Idle);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void Editor_PathBuilder::Show_PathMissionPanel()
	{
		AppContext* ctx = mApp->GetContext(); 
		PathMission& mission = ctx->GetPathMission();
		TerrainScaler& scaler = ctx->GetTerrainScaler();

		uavpf::NavGridSpecification gridSpec = mission.GetNavGrid().GetSpecification();
		glm::ivec2 resolution{ gridSpec.Width, gridSpec.Depth };

		glm::vec2 relativeStart = mission.GetRelativeStart();
		glm::vec2 relativeEnd = mission.GetRelativeEnd();

		ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
		if (State::PathFinding == mState)
		{
			sliderFlags |= ImGuiSliderFlags_ReadOnly;
		}

		ImGui::Begin("PathMission");
		bool updateNavGrid = ImGui::DragInt2("NavGrid resolution", glm::value_ptr(resolution), 1.0f, 0, 1000, "%d", sliderFlags);
		bool updateTargets = ImGui::DragFloat2("Start", glm::value_ptr(relativeStart), 0.01f, 0, 0.9f, "%.2f", sliderFlags);
		updateTargets = ImGui::DragFloat2("End", glm::value_ptr(relativeEnd), 0.01f, 0, 0.9f, "%.2f", sliderFlags) | updateTargets;

		if (ImGui::Button("Build") && mState != State::PathFinding)
		{
			mProgressSeconds = 0.0f;
			mProgressTitle = "Path finding";
			mProgressChar = ".";
			mProgressMessage = mProgressTitle;

			mission.GetPath().Clear();
			mission.SetStatus(PathMission::PathStatus::None);

			mApp->GetService()->AddTask(
				[this]()
				{
					mState = State::PathFinding;
					FindPath();
					mState = State::Idle;
				}
			);
		}

		float maxHeightKilometers = 2.0f * scaler.GetMaxHeight().GetKilometers();
		float minElevationKilometers = maxHeightKilometers * mission.GetMinElevation();
		ImGui::DragFloat("Elevation", &minElevationKilometers, 0.5f, 0.0f, maxHeightKilometers, "%.3f", sliderFlags);
		mission.SetMinElevation(minElevationKilometers / maxHeightKilometers);
		
		if (State::PathFinding == mState)
		{
			ImGui::TextColored(ImVec4(0.0f, 0.3f, 0.7f, 1.0f), "%s", mProgressMessage.data());
		}
		ImGui::End();

		if (State::PathFinding == mState)
		{
			return;
		}

		if (updateNavGrid || updateTargets)
		{
			mApp->GetContext()->GetPathMission().GetPath().Clear();
			mApp->GetContext()->GetPathMission().SetStatus(PathMission::PathStatus::None);
		}

		if (updateNavGrid)
		{
			gridSpec.Width = resolution.x;
			gridSpec.Depth = resolution.y;
			mission.SpecifyGrid(gridSpec);
		}

		mission.SetRelativeStart(relativeStart);
		mission.SetRelativeEnd(relativeEnd);
	}

	void Editor_PathBuilder::Show_ScenePanel()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5.0f);
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoDecoration & ~(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse));
		ImGui::PopStyleVar(3);

		mCanOperateCamera = ImGui::IsWindowHovered() && mApp->GetService()->GetMouse()->IsButtonPressed(MouseButton::Right);

		ImVec2 size = ImGui::GetWindowSize();

		if (size.x != mColorBuffer->GetParams().Width || size.y != mColorBuffer->GetParams().Height)
		{
			SetupRenderBuffers(size.x, size.y);

			mFreeCamera.SetProjectionMatrix(
				glm::perspective(
					glm::radians(60.0f),
					size.x / size.y,
					0.1f,
					100.0f));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImTextureID sceneTexture = ImTextureID(mApp->GetService()->GetImGuiGraphicsBackend()->GetTextureID(mColorBuffer.get()));
		ImGui::Image(sceneTexture, size, { 0, 1 }, { 1, 0 });
		ImGui::PopStyleVar(2);
		ImGui::End();
	}

	void Editor_PathBuilder::Show_MapPanel()
	{
		ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
		if (mState == State::PathFinding)
		{
			sliderFlags |= ImGuiSliderFlags_ReadOnly;
		}

		TerrainScaler& scaler = mApp->GetContext()->GetTerrainScaler();

		glm::vec4 scale;
		scale.x = scaler.GetWidth().GetKilometers();
		scale.z = scaler.GetDepth().GetKilometers();
		scale.y = scaler.GetMaxHeight().GetKilometers();
		scale.w = scaler.GetScale().GetKilometers();
		bool updated = false;

		ImGui::Begin("Terrain");
		updated = ImGui::DragFloat3("Dimensions", glm::value_ptr(scale), 2.0f, 1.0f, 1e5f, "%.3f", sliderFlags);
		ImGui::SameLine();
		if (ImGui::Button("Reset") && mState != State::PathFinding)
		{
			ResetTerrainScaling();
		}

		updated = updated | ImGui::DragFloat("Scale", &scale.w, 0.1f, 0.1f, 50.0f, "%.3f", sliderFlags);
		ImGui::End();

		if (updated)
		{
			scaler.SetWidth(DistanceMetric::FromKilometers(scale.x));
			scaler.SetDepth(DistanceMetric::FromKilometers(scale.z));
			scaler.SetMaxHeight(DistanceMetric::FromKilometers(scale.y));
			scaler.SetScale(DistanceMetric::FromKilometers(scale.w));
		}
	}

	void Editor_PathBuilder::UpdateProgressMessage()
	{
		mProgressSeconds += mApp->GetService()->GetDeltaTime();

		if (mProgressSeconds > 0.1f)
		{
			mProgressSeconds = 0.0f;
			mProgressMessage += mProgressChar;
		}

		if (mProgressMessage.length() > 35)
		{
			mProgressMessage = mProgressTitle;
		}
	}

	void Editor_PathBuilder::InitializePathMission()
	{
		AppContext* ctx = mApp->GetContext();

		uavpf::TiffImage* mapImage = ctx->GetMapImage();
		
		uavpf::HeightMap heightMap = uavpf::HeightMapBuilder()
			.SetGrayscale(uavpf::ImageGrayscale(*mapImage))
			.SetRasterSpace(uavpf::RasterSpace::RasterIsPoint)
			.Build();

		ctx->GetPathMission().SetHeightMap(std::move(heightMap));
	}

	void Editor_PathBuilder::InitializeTerrainMesh()
	{
		AppContext* ctx = mApp->GetContext();
		const uavpf::HeightMap* heightMap = ctx->GetPathMission().GetNavGrid().GetHeightMap();

		mTerrainMesh = uavpf::TerrainMeshBuilder()
			.SetHeight(*heightMap)
			.Build();

		ResetTerrainScaling();

		ctx->GetTerrainScaler().SetWidth(DistanceMetric::FromKilometers(heightMap->GetWidth()));
		ctx->GetTerrainScaler().SetDepth(DistanceMetric::FromKilometers(heightMap->GetDepth()));

		mCanLoadMesh = true;
	}

	void Editor_PathBuilder::FindPath()
	{
		PathMission& mission = mApp->GetContext()->GetPathMission();
		uavpf::NavGrid navGrid(mission.GetNavGrid());

		uavpf::ElevationConservingCost cost;
		// uavpf::ContourMatchingCost cost;
		uavpf::AStarAlgorithm pathFinder(&cost, &navGrid, mission.GetStart(), mission.GetEnd());

		while (pathFinder.IsExplorable())
		{
			pathFinder.ExploreNext();
			if (pathFinder.IsGoal())
			{
				mission.SetStatus(PathMission::PathStatus::Found);
				break;
			}

			glm::ivec2 directions[]
			{
				{  0,  1 },
				{  0, -1 },
				{  1,  0 },
				{ -1,  0 },

				{  1,  1 },
				{  1, -1 },
				{ -1,  1 },
				{ -1,  -1 },
			};

			for (const glm::ivec2& direction : directions)
			{
				pathFinder.ExploreNeighbour(direction);
			}
		}

		if (mission.GetStatus() != PathMission::PathStatus::Found)
		{
			mission.SetStatus(PathMission::PathStatus::NotFound);
			return;
		}

		for (uavpf::NavNode* node : pathFinder.ConstructPath())
		{
			glm::ivec2 navCoords = navGrid.GetCoordinates(node);
			mission.GetPath().AddCoordinate(navCoords, navGrid.GetElevation(navCoords) + mission.GetMinElevation());
		}
	}

	void Editor_PathBuilder::LoadTerrainMesh()
	{
		if (!mCanLoadMesh || mTerrainRenderMesh != nullptr)
		{
			return;
		}

		mTerrainRenderMesh = std::make_unique<TerrainRenderMesh>(
			mApp->GetService()->GetGraphics(),
			mTerrainMesh);

		glm::mat4 model = mApp
			->GetContext()
			->GetMapImage()
			->GetTag(uavpf::TiffTag::Geo_ModelTransformationTag)
			->AsMatrix();
	}

	void Editor_PathBuilder::SetupRenderBuffers(int32_t width, int32_t height)
	{
		GraphicsContext* graphics = mApp->GetService()->GetGraphics();

		Texture2DParams params;
		params.Width = width;
		params.Height = height;
		params.DebugName = "Editor - Color buffer";
		params.Format = GraphicsFormat::R8G8B8A8_UNORM;

		mColorBuffer.reset(
			graphics->CreateTexture2D(std::move(params)),
			[graphics](Texture2D* texture)
			{
				graphics->DestroyTexture2D(texture);
			}
		);

		params.DebugName = "Editor - Depth buffer";
		params.Format = GraphicsFormat::D32_FLOAT;

		mDepthBuffer.reset(
			graphics->CreateTexture2D(std::move(params)),
			[graphics](Texture2D* texture)
			{
				graphics->DestroyTexture2D(texture);
			}
		);
	}

	void Editor_PathBuilder::SetupRenderers()
	{
		GraphicsContext* graphics = mApp->GetService()->GetGraphics();

		mTerrainRenderer = std::make_unique<TerrainRenderer>(graphics);
		mOverlayRenderer = std::make_unique<OverlayRenderer>(graphics);
	}

	void Editor_PathBuilder::HandleCameraInput()
	{
		if (!mCanOperateCamera)
		{
			return;
		}

		float dt = mApp->GetService()->GetDeltaTime();
		float velocity = 10.0f;
		glm::vec3 moveDirection(0.0f);
		Keyboard* kb = mApp->GetService()->GetKeyboard();

		if (kb->IsKeyDown(Key::W))
		{
			moveDirection += mFreeCamera.GetFrontVector();
		}
		if (kb->IsKeyDown(Key::A))
		{
			moveDirection -= mFreeCamera.GetRightVector();
		}
		if (kb->IsKeyDown(Key::S))
		{
			moveDirection -= mFreeCamera.GetFrontVector();
		}
		if (kb->IsKeyDown(Key::D))
		{
			moveDirection += mFreeCamera.GetRightVector();
		}

		moveDirection = glm::length(moveDirection) > glm::epsilon<float>()
			? glm::normalize(moveDirection)
			: moveDirection;

		mFreeCamera.SetPosition(mFreeCamera.GetPosition() + moveDirection * velocity * dt);
	}

	void Editor_PathBuilder::ResetTerrainScaling()
	{
		AppContext* ctx = mApp->GetContext();
		const uavpf::HeightMap* heightMap = ctx->GetPathMission().GetNavGrid().GetHeightMap();

		ctx->GetTerrainScaler().SetWidth(DistanceMetric::FromKilometers(heightMap->GetWidth()));
		ctx->GetTerrainScaler().SetDepth(DistanceMetric::FromKilometers(heightMap->GetDepth()));
	}

	glm::vec4 Editor_PathBuilder::NavCoordToWorldCoord(const glm::vec3& navCoord) const
	{
		TerrainScaler& scaler = mApp->GetContext()->GetTerrainScaler();
		PathMission& mission = mApp->GetContext()->GetPathMission();
		const uavpf::NavGrid& navGrid = mission.GetNavGrid();

		glm::vec2 navCoordsWithoutHeight(navCoord.x, navCoord.z);
		glm::ivec2 imageCoord = navGrid.ConvertCoordinates(navCoordsWithoutHeight);
		glm::vec3 imageSpaceCoord(imageCoord.x, navCoord.y, imageCoord.y);
		glm::vec4 coord = scaler.GetModelMatrix() * glm::vec4(imageSpaceCoord, 1.0f);

		return coord;
	}

	bool Editor_PathBuilder::OnMouseMove(const MouseMovementEvent& event)
	{
		if (!mCanOperateCamera)
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

