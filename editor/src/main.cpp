#include <uavpf/uavpf.h>

#include "event/event_bus.h"
#include "event/event_queue.h"
#include "event/event_subscriber.h"
#include "graphics/shader_compiler.h"
#include "graphics/subresource.h"
#include "platform/platform_events.h"
#include "platform/platform_service.h"
#include "rendering/camera.h"
#include "rendering/frame_timer.h"
#include "uavpf/debug/concise_log_formatter.h"
#include "uavpf/terrain/height_map.h"
#include "uavpf/terrain/terrain_mesh.h"

namespace editor
{
	class TestApplication
	{
	public:
		TestApplication()
		{
			mEvents = EventBus(mEventQueue, mDispatcher);
			mSubscriber = EventSubscriber(mEvents);

			mSubscriber
				.BeginClass(*this)
					.SubscribeMethod(&TestApplication::OnWindowClose)
					.SubscribeMethod(&TestApplication::OnWindowResize)
					.SubscribeMethod(&TestApplication::OnMouseButtonDown)
					.SubscribeMethod(&TestApplication::OnMouseButtonUp)
					.SubscribeMethod(&TestApplication::OnMouseMovement)
					.SubscribeMethod(&TestApplication::OnKeyDown)
					.SubscribeMethod(&TestApplication::OnKeyUp)
				.EndClass();

			mPlatform = CreatePlatformService();
			mPlatform->BindEvents(mEvents);

			mWindow = mPlatform->CreateWindow();
			mWindow->SetTitle("Editor");

			mMouse = mPlatform->CreateMouse();

			mKeyboard = mPlatform->CreateKeyboard();

			{
				uavpf::TiffImage image = uavpf::TiffLoader().LoadImageFromFile("C:/Users/Leonid/Desktop/terrain.tif");
				uavpf::ImageLuminance lum(image);
				mHeights = uavpf::HeightMap::FromLuminance(lum);
				mTerrain = uavpf::TerrainMeshBuilder(mHeights)
					.Build();
			}

			GraphicsContextParams graphicsParams;
			graphicsParams.OutputWindow = mWindow;
			graphicsParams.Backend = GraphicsBackend::OpenGL;
			graphicsParams.Kind = GraphicsContextKind::Debug;
			mGraphics = mPlatform->CreateGraphicsContext(std::move(graphicsParams));

			mGraphicsDebug = mGraphics->CreateDebugWatch();
			mGraphicsDebug->SetWatcher(
				[](const GraphicsDebugEntry& entry)
				{
					UAVPF_LOG(
						Application,
						Info,
						entry.Message);
				}
			);

			GraphicsBufferParams vertexBufferParams;
			vertexBufferParams.DebugName = "Terrain vertices";
			vertexBufferParams.StructSize = sizeof(float[4]);
			vertexBufferParams.StructCount = mTerrain.GetPositionCount();
			vertexBufferParams.Target = GraphicsBufferTarget::Vertex;

			mVertexBuffer = mGraphics->CreateBuffer(std::move(vertexBufferParams));
			mVertexBuffer->SetData(mTerrain.GetPositionData(), mTerrain.GetPositionCount() * sizeof(glm::vec4));

			GraphicsBufferParams indexBufferParams;
			indexBufferParams.DebugName = "Terrain indices";
			indexBufferParams.StructSize = sizeof(uint32_t);
			indexBufferParams.StructCount = mTerrain.GetIndexCount();
			indexBufferParams.Target = GraphicsBufferTarget::Index;

			mIndexBuffer = mGraphics->CreateBuffer(std::move(indexBufferParams));
			mIndexBuffer->SetData(mTerrain.GetIndexData(), mTerrain.GetIndexCount() * sizeof(uint32_t));

			VertexInputParams triangleVertexInputParams;	
			triangleVertexInputParams.DebugName = "Terrain vertex input";
			triangleVertexInputParams.VertexBuffers[0] = mVertexBuffer;
			triangleVertexInputParams.IndexBuffer = mIndexBuffer;
			triangleVertexInputParams.VertexAttributes =
			{
				{ "Position", GraphicsFormat::R32G32B32A32_FLOAT, 0 },
			};

			mVertexInput = mGraphics->CreateVertexInput(std::move(triangleVertexInputParams));

			mShaderCompiler = mGraphics->GetShaderCompiler();

			std::string_view vsSource =
				R"(
				#version 460 core

				uniform mat4 uModel;
				uniform mat4 uViewProj;

				layout (location = 0) in vec3 aPosition;

				out vec4 modelSpacePosition;

				out gl_PerVertex
				{
					vec4 gl_Position;
				};

				void main()
				{
					modelSpacePosition = vec4(aPosition, 1.0);
					gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
				}
				)";
			std::string_view psSource =
				R"(
				#version 460 core

				in vec4 modelSpacePosition;

				out vec4 oColor;

				void main()
				{
					oColor = vec4(vec3(modelSpacePosition.y), 1.0);
				}
				)";

			ShaderCompilation* compilation = mShaderCompiler->Compile(ShaderKind::Vertex, vsSource);	
			mVertexShader = mGraphics->CreateShader(compilation);	
			mShaderCompiler->DestroyCompilation(compilation);

			compilation = mShaderCompiler->Compile(ShaderKind::Pixel, psSource);
			mPixelShader = mGraphics->CreateShader(compilation);
			mShaderCompiler->DestroyCompilation(compilation);

			Viewport vp;
			vp.Width = mWindow->GetWidth();
			vp.Height = mWindow->GetHeight();

			DepthStencilState depthStencilState;
			depthStencilState.DepthTestEnabled = true;

			mGraphics->SetVertexInput(mVertexInput);
			mGraphics->SetShader(ShaderKind::Vertex, mVertexShader);
			mGraphics->SetShader(ShaderKind::Pixel, mPixelShader);
			mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleStrip);
			mGraphics->SetViewport(vp);
			mGraphics->SetDepthStencilState(depthStencilState);

			glm::mat4 scale(1.0f);
			scale = glm::scale(scale, glm::vec3(0.01f, 1.7f, 0.01f));
			mVertexShader->SetUniform("uModel", scale);

			mCamera.SetPosition({ 0.0f, 0.0f, 3.0f });
			mCamera.SetProjectionMatrix(glm::perspective(
				glm::radians(60.0f),
				16.0f / 9.0f,
				0.1f,
				1000.0f));
			mCamera.LookAt({ 0.0f, 0.0f, 0.0f });
		}

		~TestApplication()
		{
			mGraphics->DestroyShader(mPixelShader);
			mPixelShader = nullptr;

			mGraphics->DestroyShader(mVertexShader);
			mVertexShader = nullptr;

			mGraphics->DestroyVertexInput(mVertexInput);
			mVertexInput = nullptr;

			mGraphics->DestroyBuffer(mIndexBuffer);
			mIndexBuffer = nullptr;

			mGraphics->DestroyBuffer(mVertexBuffer);
			mVertexBuffer = nullptr;

			mGraphics->DestroyDebugWatch(mGraphicsDebug);
			mGraphicsDebug = nullptr;

			mPlatform->DestroyGraphicsContext(mGraphics);
			mGraphics = nullptr;

			mPlatform->DestroyKeyboard(mKeyboard);
			mKeyboard = nullptr;

			mPlatform->DestroyMouse(mMouse);
			mMouse = nullptr;

			mPlatform->DestroyWindow(mWindow);
			mWindow = nullptr;

			DestroyPlatformService(mPlatform);
			mPlatform = nullptr;
		}

		void StartMainLoop()
		{
			mRunning = true;
			while (mRunning)
			{
				mTimer.Tick();
				const float dt = mTimer.GetDeltaTimeSeconds();

				mPlatform->PollEvents();
				mEvents.Dispatch();

				glm::vec3 cameraSpeed = glm::vec3(0.0f);
				if (mKeyboard->IsKeyDown(Key::W))
				{
					cameraSpeed += mCamera.GetFrontVector();
				}
				if (mKeyboard->IsKeyDown(Key::S))
				{
					cameraSpeed -= mCamera.GetFrontVector();
				}
				if (mKeyboard->IsKeyDown(Key::A))
				{
					cameraSpeed -= mCamera.GetRightVector();
				}
				if (mKeyboard->IsKeyDown(Key::D))
				{
					cameraSpeed += mCamera.GetRightVector();
				}

				cameraSpeed = glm::length(cameraSpeed) > 0.0f
					? glm::normalize(cameraSpeed)
					: glm::vec3(0.0f);

				mCamera.SetPosition(mCamera.GetPosition() + cameraSpeed * dt);
			
				mVertexShader->SetUniform("uViewProj", mCamera.CalculateViewProjectionMatrix());

				mGraphics->SetFramebuffer(nullptr);
				mGraphics->ClearColor(nullptr, 0.7f, 0.4f, 0.3f, 0.0f);
				mGraphics->ClearDepthStencil(nullptr, 1.0f, 0);

				for (int strip = 0; strip < mTerrain.GetNumberOfTriangleStrips(); strip++)
				{
					mGraphics->DrawIndexed(
						mTerrain.GetNumberOfVerticesPerTriangleStrip() * strip,
						mTerrain.GetNumberOfVerticesPerTriangleStrip());
				}

				mGraphics->Present();
			}
		}

	private:
		bool OnWindowClose(const WindowCloseEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Window '%s' closed!",
				event.ClosedWindow->GetTitle().data());

			mRunning = false;

			return true;
		}

		bool OnWindowResize(const WindowResizeEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Window '%s' is resized: %dx%d",
				event.ResizedWindow->GetTitle().data(),
				event.ResizedWindow->GetWidth(),
				event.ResizedWindow->GetHeight());

			Viewport vp;
			vp.Width = event.Width;
			vp.Height = event.Height;
			mGraphics->SetViewport(vp);

			return true;
		}

		bool OnMouseButtonDown(const MouseButtonDownEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Button: '%i' is down",
				event.ButtonDown);
			return true;
		}

		bool OnMouseButtonUp(const MouseButtonUpEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Button: '%i' is up",
				event.ButtonUp);
			return true;
		}

		bool OnMouseMovement(const MouseMovementEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Mouse moved: %dx%d",
				event.DeltaX,
				event.DeltaY);

			auto [polar, azimuth] = mCamera.CalculatePolarAndAzimuth();
			
			polar += event.DeltaY * 0.01f;
			azimuth += event.DeltaX * 0.01f;

			polar = glm::clamp(polar, 0.1f, glm::pi<float>() - 0.1f);

			mCamera.LookAround(polar, azimuth);

			return true;
		}

		bool OnKeyDown(const KeyDownEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Key down: %d", event.KeyDown);
			return true;
		}

		bool OnKeyUp(const KeyUpEvent& event)
		{
			UAVPF_LOG(
				Application,
				Info,
				"Key up: %d", event.KeyUp);
			return true;
		}

		bool mRunning = true;

		// Event-system
		EventQueue mEventQueue;
		EventDispatcher mDispatcher;
		EventBus mEvents;
		EventSubscriber mSubscriber;

		// Platform
		PlatformService* mPlatform = nullptr;
		Window* mWindow = nullptr;
		Mouse* mMouse = nullptr;
		Keyboard* mKeyboard = nullptr;

		// Terrain
		uavpf::HeightMap mHeights;
		uavpf::TerrainMesh mTerrain;

		// Graphics
		GraphicsContext* mGraphics = nullptr;
		GraphicsDebugWatch* mGraphicsDebug = nullptr;
		GraphicsBuffer* mVertexBuffer = nullptr;
		GraphicsBuffer* mIndexBuffer = nullptr;
		VertexInput* mVertexInput = nullptr;
		ShaderCompiler* mShaderCompiler = nullptr;
		Shader* mVertexShader = nullptr;
		Shader* mPixelShader = nullptr;

		Camera mCamera;
		FrameTimer mTimer;
	};
}
int main()
{
	auto formatter = std::make_unique<uavpf::ConciseLogFormatter>();
	auto emitter = std::make_unique<uavpf::ConsoleLogEmitter>();

	uavpf::LoggerProvider::Get().HostLogger(
		uavpf::LogSource::Application,
		std::move(formatter),
		std::move(emitter));

	editor::TestApplication().StartMainLoop();
}

