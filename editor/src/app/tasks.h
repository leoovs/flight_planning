#pragma once

#include <string>

#include "app/app_events.h"
#include "app/path_mission.h"
#include "event/event_publisher.h"
#include "tasking/task.h"
#include "event/event_bus.h"
#include "event/event_subscriber.h"

namespace editor
{
	class UpdateProgressBarTask final : public Task
	{
	public:
		UpdateProgressBarTask(EventBus events);
		~UpdateProgressBarTask() override = default;

		void Start() override;
		void Abort() override;
		void Update() override;

		bool IsDone() const override;

	private:
		bool OnCancelPathFinding(const CancelPathFindingEvent& event);

		bool mCancel = false;
		std::string mProgressBar = "";
		EventSubscriber mSubscriber;
	};

	class FindPathTask final : public Task
	{
	public:
		FindPathTask(PathMission& mission, EventBus bus);
		~FindPathTask() override = default;

		void Start() override;
		void Abort() override;
		void Update() override;

		bool IsDone() const override;

	private:
		void ConstructPath();
		void ResetNavGrid();

		bool OnCancelPathFinding(const CancelPathFindingEvent& event);

		bool mDone = false;
		PathMission* mMission = nullptr;
		uavpf::NavGrid mNavGrid;
		std::unique_ptr<uavpf::AStarAlgorithm> mAlgorithm;
		EventSubscriber mSubscriber;
		EventPublisher mPublisher;
	};
}

