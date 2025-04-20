#include "application.h"
#include "platform/platform_service.h"
#include "rendering/overlay_renderer.h"
#include "uavpf/algo/astar_algorithm.h"
#include "uavpf/algo/navgrid.h"
#include "uavpf/debug/logger_provider.h"

#include <glm/gtx/string_cast.hpp>

namespace editor 
{
	Application::Application()
	{
		SubscribeEvents();

		SetupPlatform();
		SetupGraphics();
		SetupRenderer();
		SetupAlgorithm();
	}

	Application::~Application()
	{
		ShutDownAlgorithm();
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
		speed = glm::length(speed) > glm::epsilon<float>()
			? glm::normalize(speed)
			: speed;
		mCamera.SetPosition(mCamera.GetPosition() + 4.0f * speed * mFrameTimer.GetDeltaTimeSeconds());

		uavpf::AStarAlgorithm algo;
		
		while (algo.IsExplorable())
		{
			algo.ExploreNext();

			if (algo.IsGoal())
			{
				break;
			}
		}
	}

	void Application::Render()
	{
		mGraphics->SetViewport({ 0, 0, mWindow->GetWidth(), mWindow->GetHeight() });
		mGraphics->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
		mGraphics->ClearDepthStencil(nullptr, 1.0f, 0);

		glm::mat4 scale(1.0f);
		scale = glm::scale(scale, glm::vec3(0.01f, 4.0f, 0.01f));

		mRenderer->SetCamera(mCamera);
		mOverlay->SetCamera(mCamera);

		for (uavpf::NavNode* node : mPath)
		{
			glm::ivec2 navCoords = mGrid->GetCoordinates(node);
			float elevation = mGrid->GetElevation(navCoords.x, navCoords.y);
			glm::ivec2 imageCoords = mGrid->ConvertCoordinates(navCoords);

			glm::vec3 coords(imageCoords.x, elevation, imageCoords.y);
			glm::vec3 worldCoords = scale * glm::vec4(coords, 1.0f);

			mOverlay->RenderCircle3D(worldCoords + glm::vec3(0.0f, 0.03f, 0.0f), 0.03f, glm::vec3(1.0f));
		}

		mRenderer->Render(mRenderMesh, scale);

		mGraphics->Present();
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

	void Application::SetupAlgorithm()
	{
		uavpf::NavGridSpecification spec;
		spec.Width = 100;
		spec.Depth = 100;

		mGrid = new uavpf::NavGrid(spec);
		mGrid->SetHeightMap(&mHeightMap);

		mPathFinder.Initialize(mGrid, { 0, 0 }, { 50, 50 });

		bool pathFound = false;
		while (mPathFinder.IsExplorable())
		{
			mPathFinder.ExploreNext();
			if (mPathFinder.IsGoal())
			{
				pathFound = true;
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
				mPathFinder.ExploreNeighbour(direction);
			}
		}

		if (!pathFound)
		{
			UAVPF_LOG(Application, Info, "Path not found");
		}

		mPath = mPathFinder.ConstructPath();

		for (uavpf::NavNode* node : mPath)
		{
			auto navCoords = mGrid->GetCoordinates(node);
			auto imageCoords = mGrid->ConvertCoordinates(navCoords); 

			UAVPF_LOG(
				Application,
				Info,
				"%s",
				glm::to_string(imageCoords).data());
		}
	}

	void Application::ShutDownAlgorithm()
	{
		delete mGrid;
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
		auto [polar, azimuth] = mCamera.CalculatePolarAndAzimuth();

		polar = glm::clamp(polar + event.DeltaY * 0.01f, 0.01f, glm::pi<float>() - 0.1f);
		azimuth += event.DeltaX * 0.01f;

		mCamera.LookAround(polar, azimuth);

		return true;
	}
}

