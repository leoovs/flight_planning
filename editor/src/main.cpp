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

			struct Vertex
			{
				float position[3];
			};

			Vertex triangle[]
			{
				{ -0.5f, -0.5f, 0.0f },
				{  0.0f,  0.5f, 0.0f },
				{  0.5f, -0.5f, 0.0f },
			};

			GraphicsBufferParams vertexBufferParams;
			vertexBufferParams.DebugName = "SUPER COOL vertex buffer";
			vertexBufferParams.StructSize = sizeof(*triangle);
			vertexBufferParams.StructCount = std::size(triangle);
			vertexBufferParams.Target = GraphicsBufferTarget::Vertex;

			mVertexBuffer = mGraphics->CreateBuffer(std::move(vertexBufferParams));
			mVertexBuffer->SetData(triangle, sizeof(triangle));

			GraphicsBufferParams constantBufferParams;
			constantBufferParams.DebugName = "Test constant buffer";
			constantBufferParams.StructSize = sizeof(float[4]);
			constantBufferParams.StructCount = 1;
			constantBufferParams.Target = GraphicsBufferTarget::Constant;

			VertexInputParams triangleVertexInputParams;	
			triangleVertexInputParams.DebugName = "Triangle vertex input";
			triangleVertexInputParams.VertexBuffers[0] = mVertexBuffer;
			triangleVertexInputParams.IndexBuffer = nullptr;
			triangleVertexInputParams.VertexAttributes =
			{
				{ "Position", GraphicsFormat::R32G32B32_FLOAT, 0 },
			};

			mTriangleVertexInput = mGraphics->CreateVertexInput(std::move(triangleVertexInputParams));

			mShaderCompiler = mGraphics->GetShaderCompiler();

			std::string_view vsSource =
				R"(
				#version 460 core

				uniform mat4 uViewProj;

				layout (location = 0) in vec3 aPosition;

				out gl_PerVertex
				{
					vec4 gl_Position;
				};

				void main()
				{
					gl_Position = uViewProj * vec4(aPosition, 1.0);
				}
				)";
			std::string_view psSource =
				R"(
				#version 460 core

				out vec4 oColor;

				uniform vec3 uColor;

				void main()
				{
					oColor = vec4(uColor, 1.0);
				}
				)";

			ShaderCompilation* compilation = mShaderCompiler->Compile(ShaderKind::Vertex, vsSource);	
			mVertexShader = mGraphics->CreateShader(compilation);	
			mShaderCompiler->DestroyCompilation(compilation);

			compilation = mShaderCompiler->Compile(ShaderKind::Pixel, psSource);
			mPixelShader = mGraphics->CreateShader(compilation);
			mPixelShader->SetUniform("uColor", 1.0f, 0.5f, 1.0f);
			mShaderCompiler->DestroyCompilation(compilation);

			Viewport vp;
			vp.Width = mWindow->GetWidth();
			vp.Height = mWindow->GetHeight();

			Texture2DParams testTextureParams;
			testTextureParams.DebugName = "Render Texture";
			testTextureParams.Width = 720;
			testTextureParams.Height = 720;
			testTextureParams.MipLevelCount = 1;
			testTextureParams.Format = GraphicsFormat::R8G8B8A8_UNORM;
			mTestTexture = mGraphics->CreateTexture2D(std::move(testTextureParams));

			FramebufferParams offscreenFramebufferParams;
			offscreenFramebufferParams.DebugName = "Offscreen framebuffer";
			mOffscreenFramebuffer = mGraphics->CreateFramebuffer(std::move(offscreenFramebufferParams));
			mOffscreenFramebuffer->AttachTexture2D(FramebufferAttachment::Color, mTestTexture, {});

			mGraphics->SetVertexInput(mTriangleVertexInput);
			mGraphics->SetShader(ShaderKind::Vertex, mVertexShader);
			mGraphics->SetShader(ShaderKind::Pixel, mPixelShader);
			mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleList);
			mGraphics->SetViewport(vp);
			mGraphics->SetFramebuffer(mOffscreenFramebuffer);

			mCamera.SetPosition({ 0.0f, 0.0f, 3.0f });
			mCamera.SetProjectionMatrix(glm::perspective(
				glm::radians(60.0f),
				16.0f / 9.0f,
				0.001f,
				1000.0f));
			mCamera.LookAt({ 0.0f, 0.0f, 0.0f });
		}

		~TestApplication()
		{
			mGraphics->DestroyFramebuffer(mOffscreenFramebuffer);
			mOffscreenFramebuffer = nullptr;

			mGraphics->DestroyTexture2D(mTestTexture);
			mTestTexture = nullptr;

			mGraphics->DestroyShader(mPixelShader);
			mPixelShader = nullptr;

			mGraphics->DestroyShader(mVertexShader);
			mVertexShader = nullptr;

			mGraphics->DestroyVertexInput(mTriangleVertexInput);
			mTriangleVertexInput = nullptr;

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
				mGraphics->Draw(0, 3);

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

		// Graphics
		GraphicsContext* mGraphics = nullptr;
		GraphicsDebugWatch* mGraphicsDebug = nullptr;
		GraphicsBuffer* mVertexBuffer = nullptr;
		VertexInput* mTriangleVertexInput = nullptr;
		ShaderCompiler* mShaderCompiler = nullptr;
		Shader* mVertexShader = nullptr;
		Shader* mPixelShader = nullptr;
		Texture2D* mTestTexture = nullptr;
		Framebuffer* mOffscreenFramebuffer = nullptr;

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

