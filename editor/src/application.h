#pragma once

#include "event/event_bus.h"
#include "event/event_queue.h"
#include "event/event_dispatcher.h"
#include "event/event_subscriber.h"
#include "graphics/graphics_context.h"
#include "platform/platform_events.h"
#include "platform/platform_service.h"
#include "rendering/frame_timer.h"
#include "rendering/terrain_renderer.h"

namespace editor
{
	class Application final
	{
	public:
		Application();
		~Application();

		void Run();
		void Quit();
		void Update(float dt);
		void Render();

	private:
		void SubscribeEvents();

		void SetupPlatform();
		void SetupGraphics();
		void SetupRenderer();

		void ShutDownRenderer();
		void ShutDownGraphics();
		void ShutDownPlatform();

		bool OnWindowClose(const WindowCloseEvent& event);
		bool OnMouseMove(const MouseMovementEvent& event);

		bool mRunning = false;

		EventQueue mEventQueue;
		EventDispatcher mEventDispatcher;
		EventBus mEvents;
		EventSubscriber mEventSubscriber;

		PlatformService* mPlatform = nullptr;
		Mouse* mMouse = nullptr;
		Keyboard* mKeyboard = nullptr;
		Window* mWindow = nullptr;

		GraphicsContext* mGraphics = nullptr;
		GraphicsDebugWatch* mGraphicsWatch = nullptr;
		Texture2D* mColorBuffer = nullptr;
		Texture2D* mDepthBuffer = nullptr;

		TerrainRenderer* mRenderer = nullptr;
		uavpf::TerrainMesh mMesh;
		TerrainRenderMesh* mRenderMesh = nullptr;
		FrameTimer mFrameTimer;
		Camera mCamera;
	};
}

