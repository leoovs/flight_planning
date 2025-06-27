#include "runtime/start_up.h"

#include <imgui/imgui.h>

#include "graphics/imgui_graphics_backend.h"
#include "platform/imgui_platform_backend.h"
#include "platform/platform_service.h"
#include "runtime/main_loop.h"
#include "runtime/rt_module_locator.h"
#include "tasking/task_scheduler.h"

namespace editor
{
	void StartUp(AppFactoryFn boundApp)
	{
		PlatformService* platform = CreatePlatformService();
		Window* window = platform->CreateWindow();
		Keyboard* keyboard = platform->CreateKeyboard();
		Mouse* mouse = platform->CreateMouse();

		GraphicsContextParams graphicsParams;
#ifdef NDEBUG
		graphicsParams.Kind = GraphicsContextKind::Optimal;
#else
		graphicsParams.Kind = GraphicsContextKind::Debug;
#endif
		graphicsParams.OutputWindow = window;
		graphicsParams.Backend = GraphicsBackend::OpenGL;

		GraphicsContext* graphics = platform->CreateGraphicsContext(std::move(graphicsParams));

		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGuiPlatformBackend* imguiPlatform = platform->CreateImGuiBackend();
		ImGuiGraphicsBackend* imguiGraphics = imguiPlatform->CreateGraphicsBackend(graphics);

		RtModuleLocator::Register<PlatformService>(platform);
		RtModuleLocator::Register<Window>(window);
		RtModuleLocator::Register<Keyboard>(keyboard);
		RtModuleLocator::Register<Mouse>(mouse);
		RtModuleLocator::Register<GraphicsContext>(graphics);
		RtModuleLocator::Register<ImGuiPlatformBackend>(imguiPlatform);
		RtModuleLocator::Register<ImGuiGraphicsBackend>(imguiGraphics);

		{
			TaskScheduler taskScheduler;
			RtModuleLocator::Register<TaskScheduler>(&taskScheduler);

			{
				MainLoop mainLoop(
					platform,
					&taskScheduler,
					graphics,
					imguiPlatform,
					imguiGraphics,
					std::move(boundApp));
				mainLoop.Run();
			}

			RtModuleLocator::Unregister<TaskScheduler>();
		}

		RtModuleLocator::Unregister<ImGuiGraphicsBackend>();
		RtModuleLocator::Unregister<ImGuiPlatformBackend>();
		RtModuleLocator::Unregister<GraphicsContext>();
		RtModuleLocator::Unregister<Mouse>();
		RtModuleLocator::Unregister<Keyboard>();
		RtModuleLocator::Unregister<Window>();
		RtModuleLocator::Unregister<PlatformService>();

		imguiPlatform->DestroyGraphicsBackend(imguiGraphics);
		platform->DestroyImGuiBackend(imguiPlatform);
		ImGui::DestroyContext();

		platform->DestroyGraphicsContext(graphics);
		platform->DestroyMouse(mouse);
		platform->DestroyKeyboard(keyboard);
		platform->DestroyWindow(window);
		DestroyPlatformService(platform);
	}
}

