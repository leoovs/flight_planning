#pragma once

#include <uavpf/uavpf.h>

#include "app/app.h"
#include "app/app_service.h"
#include "app/app_context.h"
#include "app/editor_state.h"
#include "event/event_publisher.h"
#include "platform/platform_events.h"

namespace editor
{
	class EditorApp final : public App
	{
	public:
		void RegisterService(AppService* service) override;
		void Bind(EventBus& bus) override;

		void OnRun() override;
		void OnUI() override;
		void Update() override;
		void Render() override;

		void Push(EditorStateKind kind);
		EventBus& GetEvents() const;
		AppService* GetService() const;
		AppContext* GetContext() const;

	private:
		void SetupStates();
		EditorState* GetCurrentState() const;

		void ShowDockspace();
		void ShowMenu();

		bool OnWindowClose(const WindowCloseEvent& event);
		bool OnWindowResize(const WindowResizeEvent& event);

		AppService* mService = nullptr;
		std::unique_ptr<AppContext> mContext;

		EventBus* mEventBus = nullptr;
		EventSubscriber mEventSubscriber;
		EventPublisher mEventPublisher;

		std::queue<EditorStateKind> mStateQueue;
		std::array<std::unique_ptr<EditorState>, size_t(EditorStateKind::Count_)> mStates;
	};
}

