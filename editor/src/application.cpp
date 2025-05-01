#include "application.h"
#include "imgui.h"
#include "platform/platform_service.h"
#include "rendering/overlay_renderer.h"
#include "uavpf/algo/astar_algorithm.h"
#include "uavpf/algo/astar_cost.h"
#include "uavpf/algo/navgrid.h"
#include <glm/gtx/string_cast.hpp>

namespace editor 
{
	Application::Application()
	{
		SubscribeEvents();

		SetupPlatform();
		SetupGraphics();
		SetupRenderer();
		SetupImGui();
		SetupAlgorithm();
	}

	Application::~Application()
	{
		ShutDownAlgorithm();
		ShutDownImGui();
		ShutDownRenderer();
		ShutDownGraphics();
		ShutDownPlatform();
	}

	void Application::Run()
	{
		mRunning = true;
		mFrameTimer.Tick();
		while (mRunning)
		{
			mFrameTimer.Tick();
			Update();
			DoGui();
			Render();
		}
	}

	void Application::Quit()
	{
		mRunning = false;
	}

	void Application::Update()
	{
		mPlatform->PollEvents();
		mEvents.Dispatch();

		glm::vec3 speed(0.0f);
		if (mKeyboard->IsKeyDown(Key::W))
		{
			speed += mCamera.GetFrontVector();
		}
		if (mKeyboard->IsKeyDown(Key::S))
		{
			speed -= mCamera.GetFrontVector();
		}
		if (mKeyboard->IsKeyDown(Key::A))
		{
			speed -= mCamera.GetRightVector();
		}
		if (mKeyboard->IsKeyDown(Key::D))
		{
			speed += mCamera.GetRightVector();
		}
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			speed = glm::vec3(0.0f);
		}
		speed = glm::length(speed) > glm::epsilon<float>()
			? glm::normalize(speed)
			: speed;
		mCamera.SetPosition(mCamera.GetPosition() + 4.0f * speed * mFrameTimer.GetDeltaTimeSeconds());
	}

	void Application::Render()
	{
		mGraphics->SetViewport({ 0, 0, mWindow->GetWidth(), mWindow->GetHeight() });
		mGraphics->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
		mGraphics->ClearDepthStencil(nullptr, 1.0f, 0);

		glm::mat4 scale(1.0f);
		scale = glm::scale(scale, glm::vec3(0.01f, 4.0f, 0.01f));

		mRenderer->SetCamera(mCamera);
		mRenderer->Render(mRenderMesh, scale);

		mOverlay->SetCamera(mCamera);

		int32_t iPath = 0;
		float maxElevation = 0.0f;
		for (; iPath < mPath.size() - 1; iPath++)
		{
			glm::vec3 nodeCoords[2];
			for (int32_t iNode = 0; iNode < 2; iNode++)
			{
				uavpf::NavNode* node = mPath.at(iPath + iNode);

				glm::ivec2 navCoords = mGrid.GetCoordinates(node);
				float elevation = mGrid.GetElevation(navCoords.x, navCoords.y);
				elevation += 0.065;
				glm::ivec2 imageCoords = mGrid.ConvertCoordinates(navCoords);

				maxElevation = std::max(elevation, maxElevation);

				glm::vec3 coords(imageCoords.x, maxElevation, imageCoords.y);
				glm::vec3 worldCoords = scale * glm::vec4(coords, 1.0f);
				nodeCoords[iNode] = worldCoords + glm::vec3(0.0f, 0.03f, 0.0f);

				glm::vec3 color;
				if (iPath == 0 && iNode == 0)
				{
					color = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else if (iPath == mPath.size() - 2 && iNode == 1)
				{
					color = glm::vec3(0.0f, 1.0f, 0.0f);
				}
				else
				{
					color = glm::vec3(1.0f);
				}

				mOverlay->RenderCircle3D(nodeCoords[iNode], 0.03f, color);
			}

			mOverlay->RenderLine3D(nodeCoords[0], nodeCoords[1], glm::vec3(1.0f));
		}

		mImGuiGraphics->RenderDrawData(ImGui::GetDrawData());

		mGraphics->Present();
	}

	void Application::DoGui()
	{
		mImGuiPlatform->NewFrame();
		mImGuiGraphics->NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();
	}

	void Application::SubscribeEvents()
	{
		mEvents = EventBus(mEventQueue, mEventDispatcher);
		mEventSubscriber = EventSubscriber(mEvents);

		mEventSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&Application::OnWindowClose)
				.SubscribeMethod(&Application::OnMouseMove)
			.EndClass();
	}

	void Application::SetupPlatform()
	{
		mPlatform = CreatePlatformService();
		mPlatform->BindEvents(mEvents);

		mMouse = mPlatform->CreateMouse();
		mKeyboard = mPlatform->CreateKeyboard();

		mWindow = mPlatform->CreateWindow();
		mWindow->SetTitle("UAV path planning");
	}

	void Application::SetupGraphics()
	{
		GraphicsContextParams graphicsParams;
		graphicsParams.Backend = GraphicsBackend::OpenGL;
		graphicsParams.Kind = GraphicsContextKind::Debug;
		graphicsParams.OutputWindow = mWindow;

		mGraphics = mPlatform->CreateGraphicsContext(std::move(graphicsParams));

		mGraphicsWatch = mGraphics->CreateDebugWatch();
		mGraphicsWatch->SetWatcher(
			[](const GraphicsDebugEntry& entry)
			{
				UAVPF_LOG(Application, Warning, "%s", entry.Message.data());
			}
		);
	}

	void Application::SetupRenderer()
	{
		mRenderer = new TerrainRenderer(mGraphics);
		mOverlay = new OverlayRenderer(mGraphics);

		uavpf::TiffImage image = uavpf::TiffLoader()
			.LoadImageFromFile("C:/Users/Leonid/Desktop/mountain.tif");

		mHeightMap = uavpf::HeightMapBuilder()
			.SetRasterSpace(uavpf::RasterSpace::RasterIsPoint)
			.SetGrayscale(uavpf::ImageGrayscale(image))
			.Build();

		uavpf::TerrainMesh mesh = uavpf::TerrainMeshBuilder()
			.SetHeight(mHeightMap)
			.SetTransformation(glm::mat4(1.0f))
			.Build();

		mRenderMesh = mRenderer->CreateTerrainRenderMesh(mesh);

		mCamera.SetProjectionMatrix(glm::perspective(
			glm::radians(45.0f),
			16.0f / 9.0f,
			0.01f,
			100.0f));
	}
	
	void Application::SetupImGui()
	{
		ImGui::CreateContext();

		mImGuiPlatform = mPlatform->CreateImGuiBackend();	
		mImGuiGraphics = mImGuiPlatform->CreateGraphicsBackend(mGraphics);
	}

	void Application::SetupAlgorithm()
	{
		uavpf::NavGridSpecification spec;
		spec.Width = 50;
		spec.Depth = 50;

		mGrid = uavpf::NavGrid(spec);
		mGrid.SetHeightMap(&mHeightMap);

		uavpf::ElevationConservingCost cost;
		// uavpf::ContourMatchingCost cost;
		uavpf::AStarAlgorithm pathFinder(&cost, &mGrid, { 0, 0 }, { 0.7 * 50, 0.5 * 50 });

		while (pathFinder.IsExplorable())
		{
			pathFinder.ExploreNext();
			if (pathFinder.IsGoal())
			{
				break;
			}

			glm::ivec2 directions[]
			{
				{ 0, 1 },
				{ 1, 0 },
				{ -1, 0 },
				{ 0, -1 },
				{ 1, 1 },
				{ -1, -1 },
				{ 1, -1 },
				{ -1, 1 },
			};

			for (const auto& direction : directions)
			{
				pathFinder.ExploreNeighbour(direction);
			}
		}

		mPath = pathFinder.ConstructPath();
	}

	void Application::ShutDownAlgorithm()
	{
	}

	void Application::ShutDownImGui()
	{
		mImGuiPlatform->DestroyGraphicsBackend(mImGuiGraphics);
		mPlatform->DestroyImGuiBackend(mImGuiPlatform);
	}

	void Application::ShutDownRenderer()
	{
		delete mOverlay;
		delete mRenderer;
	}

	void Application::ShutDownGraphics()
	{
		mGraphics->DestroyDebugWatch(mGraphicsWatch);
		mPlatform->DestroyGraphicsContext(mGraphics);
	}

	void Application::ShutDownPlatform()
	{
		mPlatform->DestroyWindow(mWindow);
		mPlatform->DestroyKeyboard(mKeyboard);
		mPlatform->DestroyMouse(mMouse);
	}

	bool Application::OnWindowClose(const WindowCloseEvent& event)
	{
		return Quit(), true;
	}

	bool Application::OnMouseMove(const MouseMovementEvent& event)
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			return true;
		}

		auto [polar, azimuth] = mCamera.CalculatePolarAndAzimuth();

		polar = glm::clamp(polar + event.DeltaY * 0.01f, 0.01f, glm::pi<float>() - 0.1f);
		azimuth += event.DeltaX * 0.01f;

		mCamera.LookAround(polar, azimuth);

		return true;
	}
}

