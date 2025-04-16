#include "application.h"
#include "platform/platform_service.h"
#include "rendering/overlay_renderer.h"
#include "uavpf/debug/logger_provider.h"

namespace editor 
{
	Application::Application()
	{
		SubscribeEvents();

		SetupPlatform();
		SetupGraphics();
		SetupRenderer();
	}

	Application::~Application()
	{
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
		mCamera.SetPosition(mCamera.GetPosition() + speed * mFrameTimer.GetDeltaTimeSeconds());
	}

	void Application::Render()
	{
		mGraphics->SetViewport({ 0, 0, mWindow->GetWidth(), mWindow->GetHeight() });
		mGraphics->ClearColor(nullptr, 0.3f, 0.3f, 0.3f, 1.0f);
		mGraphics->ClearDepthStencil(nullptr, 1.0f, 0);

		// mRenderer->SetCamera(mCamera);
		// mRenderer->Render(mRenderMesh, glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 4.0f, 0.01f)));

		mOverlay->SetCamera(mCamera);
		// mRenderer->SetCamera(mCamera);
		mOverlay->RenderLine3D(glm::vec3(0.5f), glm::vec3(-0.5f), glm::vec3(1.0f));
		mOverlay->RenderLine3D(glm::vec3(-0.5f), glm::vec3(-1.0f, 0.5f, 1.0f), glm::vec3(1.0f));
		mOverlay->RenderLine3D(glm::vec3(-1.0f, 0.5f, 1.0f), glm::vec3(0.5f), glm::vec3(1.0f));
		mOverlay->RenderCircle3D(glm::vec3( 0.5f), 0.1f, glm::vec3(1.0f));
		mOverlay->RenderCircle3D(glm::vec3(-0.5f), 0.1f, glm::vec3(1.0f));
		mOverlay->RenderCircle3D(glm::vec3(-1.0f, 0.5f, 1.0f), 0.1f, glm::vec3(1.0f));
		// mRenderer->Render(mRenderMesh, glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 3.0f, 0.01f)));

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

		uavpf::HeightMap heightMap = uavpf::HeightMapBuilder()
			.SetRasterSpace(uavpf::RasterSpace::RasterIsArea)
			.SetGrayscale(uavpf::ImageGrayscale(image))
			.Build();

		uavpf::TerrainMesh mesh = uavpf::TerrainMeshBuilder()
			.SetHeight(heightMap)
			.SetTransformation(glm::mat4(1.0f))
			.Build();

		mRenderMesh = mRenderer->CreateTerrainRenderMesh(mesh);

		mCamera.SetProjectionMatrix(glm::perspective(
			glm::radians(45.0f),
			16.0f / 9.0f,
			0.01f,
			100.0f));
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

