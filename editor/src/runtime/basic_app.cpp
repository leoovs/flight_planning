#include "runtime/basic_app.h"

#include "platform/platform_service.h"
#include "runtime/rt_module_locator.h"

namespace editor
{
	Window* BasicApp::GetMainWindow() const
	{
		return mMainWindow;
	}

	void BasicApp::Setup()
	{
		mPlatform = RtModuleLocator::Locate<PlatformService>();
		mGraphics = RtModuleLocator::Locate<GraphicsContext>();
		
		mMainWindow = mPlatform->CreateWindow();
	}

	void BasicApp::ShutDown()
	{
		mPlatform->DestroyWindow(mMainWindow);
		mMainWindow = nullptr;
	}

	void BasicApp::Update(float dt)
	{
	}

	void BasicApp::Render()
	{
	}
}

