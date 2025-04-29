#include "app/editor_state.h"

#include "app/editor_app.h"

#include <glm/gtc/type_ptr.hpp>
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
				mProgressMessage = "Preparing PathBuilder...";
				LoadTerrainMesh();
				HandleCameraInput();
				break;
		}
	}

	void Editor_PathBuilder::Render()
	{
		if (mState != State::Idle)
		{
			return;
		}

		if (nullptr == mTerrainRenderMesh)
		{
			if (mCanLoadMesh)
			{
				LoadTerrainMesh();
			}
			else
			{
				return;
			}
		}

		Viewport vp;
		vp.Width = mColorBuffer->GetParams().Width;
		vp.Height = mColorBuffer->GetParams().Height;
		mApp->GetService()->GetGraphics()->SetViewport(vp);

		mTerrainRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mTerrainRenderer->Clear();
		mTerrainRenderer->SetCamera(mFreeCamera);
		mTerrainRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mTerrainRenderer->Render(mTerrainRenderMesh.get(), glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 4.0f, 0.01f)));

		mOverlayRenderer->SetTargetTextures(mColorBuffer.get(), mDepthBuffer.get());
		mOverlayRenderer->SetCamera(mFreeCamera);
		mOverlayRenderer->RenderCircle3D({ 0.0f, 0.0f, 0.0f }, 0.03f, glm::vec3(1.0f));
		// mOverlayRenderer->RenderCircle3D({ 0.0f, -1.0f, -1.0f }, 0.03f, glm::vec3(1.0f));
		// mOverlayRenderer->RenderLine3D({ 0.0f, 0.0f, -1.0f }, { 0.0f, -1, -1 }, glm::vec3(1.0f));

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
				Show_PathMissionPanel();
				Show_ScenePanel();
				break;
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

		uavpf::NavGridSpecification gridSpec = mission.GetNavGrid().GetSpecification();
		glm::ivec2 resolution{ gridSpec.Width, gridSpec.Depth };

		ImGui::Begin("PathMission");
		bool updateNavGrid = ImGui::DragInt2("NavGrid resolution", glm::value_ptr(resolution), 1.0f, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::End();

		if (updateNavGrid)
		{
			gridSpec.Width = resolution.x;
			gridSpec.Depth = resolution.y;
			mission.SpecifyGrid(gridSpec);
		}
	}

	void Editor_PathBuilder::Show_ScenePanel()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoDecoration & ~(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse));
		ImGui::PopStyleVar(3);

		mCanOperateCamera = ImGui::IsWindowHovered() && mApp->GetService()->GetMouse()->IsButtonPressed(MouseButton::Wheel);

		ImVec2 size = ImGui::GetWindowSize();

		if (size.x != mColorBuffer->GetParams().Width || size.y != mColorBuffer->GetParams().Height)
		{
			SetupRenderBuffers(size.x, size.y);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImTextureID sceneTexture = ImTextureID(mApp->GetService()->GetImGuiGraphicsBackend()->GetTextureID(mColorBuffer.get()));
		ImGui::Image(sceneTexture, size, { 0, 1 }, { 1, 0 });
		ImGui::PopStyleVar(2);
		ImGui::End();

		mFreeCamera.SetProjectionMatrix(
			glm::perspective(
				glm::radians(60.0f),
				size.x / size.y,
				0.1f,
				100.0f));
	}

	void Editor_PathBuilder::UpdateProgressMessage()
	{
		mProgressSeconds += mApp->GetService()->GetDeltaTime();

		if (mProgressSeconds > 0.1f)
		{
			mProgressSeconds = 0.0f;
			mProgressMessage += "\n*";
		}

		if (mProgressMessage.length() > 35)
		{
			mProgressMessage = "Preparing PathBuilder...";
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

		mCanLoadMesh = true;
	}

	void Editor_PathBuilder::LoadTerrainMesh()
	{
		mTerrainRenderMesh = std::make_unique<TerrainRenderMesh>(
			mApp->GetService()->GetGraphics(),
			mTerrainMesh);
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

