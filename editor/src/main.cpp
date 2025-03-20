#include <uavpf/uavpf.h>

#include "event/event_bus.h"
#include "event/event_queue.h"
#include "event/event_subscriber.h"
#include "graphics/shader_compiler.h"
#include "graphics/subresource.h"
#include "platform/platform_events.h"
#include "platform/platform_service.h"
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
				.EndClass();

			mPlatform = CreatePlatformService();
			mPlatform->BindEvents(mEvents);

			mWindow = mPlatform->CreateWindow();
			mWindow->SetTitle("Editor");

			mMouse = mPlatform->CreateMouse();

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

			mConstantBuffer = mGraphics->CreateBuffer(std::move(constantBufferParams));
			float color[4] = { 1.0f, 0.5f, 0.5f, 1.0f };
			mConstantBuffer->SetData(color, sizeof(color));

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

				layout (location = 0) in vec3 aPosition;

				out gl_PerVertex
				{
					vec4 gl_Position;
				};

				void main()
				{
					gl_Position = vec4(aPosition, 1.0);
				}
				)";
			std::string_view psSource =
				R"(
				#version 460 core

				out vec4 oColor;

				layout (std140, binding = 1) uniform CBuffer
				{
					vec4 color;
				} cbuffer;

				void main()
				{
					oColor = cbuffer.color;
				}
				)";

			ShaderCompilation* compilation = mShaderCompiler->Compile(ShaderKind::Vertex, vsSource);	
			mVertexShader = mGraphics->CreateShader(compilation);	
			mShaderCompiler->DestroyCompilation(compilation);

			compilation = mShaderCompiler->Compile(ShaderKind::Pixel, psSource);
			mPixelShader = mGraphics->CreateShader(compilation);
			mShaderCompiler->DestroyCompilation(compilation);

			mShaderReflector = mGraphics->ReflectShader(mPixelShader);

			Viewport vp;
			vp.Width = mWindow->GetWidth();
			vp.Height = mWindow->GetHeight();

			Texture2DParams testTextureParams;
			testTextureParams.DebugName = "Test Texture";
			testTextureParams.Width = 2;
			testTextureParams.Height = 2;
			testTextureParams.MipLevelCount = 1;
			testTextureParams.Format = GraphicsFormat::R8G8B8A8_UNORM;
			mTestTexture = mGraphics->CreateTexture2D(std::move(testTextureParams));

			uint8_t pixels[]
			{
				0xFF,0x00,0x00,0xFF, 0x00,0xFF,0x00,0xFF,
				0x00,0x00,0xFF,0xFF, 0x00,0x00,0x00,0xFF,
			};

			SubresourceRegion region;
			region.Width = 2;
			region.Height = 2;
			mTestTexture->SetData(region, pixels);

			mGraphics->SetVertexInput(mTriangleVertexInput);
			mGraphics->SetShader(ShaderKind::Vertex, mVertexShader);
			mGraphics->SetShader(ShaderKind::Pixel, mPixelShader);
			mGraphics->SetPrimitiveMode(PrimitiveMode::TriangleList);
			mGraphics->SetViewport(vp);
			mGraphics->SetConstantBuffer(mConstantBuffer, 1);
		}

		~TestApplication()
		{
			mGraphics->DestroyTexture2D(mTestTexture);
			mTestTexture = nullptr;

			mGraphics->DestroyShaderReflector(mShaderReflector);
			mShaderReflector = nullptr;

			mGraphics->DestroyShader(mPixelShader);
			mPixelShader = nullptr;

			mGraphics->DestroyShader(mVertexShader);
			mVertexShader = nullptr;

			mGraphics->DestroyVertexInput(mTriangleVertexInput);
			mTriangleVertexInput = nullptr;

			mGraphics->DestroyBuffer(mConstantBuffer);
			mConstantBuffer = nullptr;

			mGraphics->DestroyBuffer(mVertexBuffer);
			mVertexBuffer = nullptr;

			mGraphics->DestroyDebugWatch(mGraphicsDebug);
			mGraphicsDebug = nullptr;

			mPlatform->DestroyGraphicsContext(mGraphics);
			mGraphics = nullptr;

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
				mPlatform->BeginFrame();

				mPlatform->PollEvents();
				mEvents.Dispatch();

				mGraphics->ClearColor(0.7f, 0.4f, 0.3f, 0.0f);
				mGraphics->Draw(0, 3);
				mGraphics->Present();

				mPlatform->EndFrame();
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

		// Graphics
		GraphicsContext* mGraphics = nullptr;
		GraphicsDebugWatch* mGraphicsDebug = nullptr;
		GraphicsBuffer* mVertexBuffer = nullptr;
		GraphicsBuffer* mConstantBuffer = nullptr;
		VertexInput* mTriangleVertexInput = nullptr;
		ShaderCompiler* mShaderCompiler = nullptr;
		ShaderReflector* mShaderReflector = nullptr;
		Shader* mVertexShader = nullptr;
		Shader* mPixelShader = nullptr;
		Texture2D* mTestTexture = nullptr;
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

