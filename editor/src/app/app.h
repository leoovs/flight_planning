#pragma once

namespace editor
{
	class EventBus;
	class AppService;

	class App
	{
	public:
		virtual ~App() = default;

		virtual void RegisterService(AppService* service) {}
		virtual void Bind(EventBus& events) {}

		virtual void OnRun() {}
		virtual void OnQuit() {}

		virtual void Update() {} 
		virtual void OnUI() {}
		virtual void Render() {}
	};
}

