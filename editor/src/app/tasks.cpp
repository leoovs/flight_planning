#include "app/tasks.h"
#include "app/path_mission.h"
#include "event/event_subscriber.h"

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// UpdateProgressBarTask
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	UpdateProgressBarTask::UpdateProgressBarTask(EventBus events)
	{
		mSubscriber = EventSubscriber(events);

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&UpdateProgressBarTask::OnCancelPathFinding)
			.EndClass();
	}

	void UpdateProgressBarTask::Start()
	{
		mProgressBar = "";
	}

	void UpdateProgressBarTask::Abort()
	{
		mCancel = true;
	}

	void UpdateProgressBarTask::Update()
	{
		mProgressBar += '*';

		if (mProgressBar.size() > 10)
		{
			mProgressBar = "";
		}
	}

	bool UpdateProgressBarTask::IsDone() const
	{
		return !mCancel;
	}

	bool UpdateProgressBarTask::OnCancelPathFinding(const CancelPathFindingEvent& event)
	{
		return Abort(), true;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// FindPathTask
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	FindPathTask::FindPathTask(PathMission& mission, EventBus bus)
		: mMission(&mission)
	{
		mSubscriber = EventSubscriber(bus);

		mSubscriber
			.BeginClass(*this)
				.SubscribeMethod(&FindPathTask::OnCancelPathFinding)
			.EndClass();
	}

	void FindPathTask::Start()
	{
		mNavGrid = mMission->GetNavGrid();

		mAlgorithm = std::make_unique<uavpf::AStarAlgorithm>(
			&mMission->GetCosts(),
			&mNavGrid,
			mMission->GetStart(),
			mMission->GetEnd());
	}

	void FindPathTask::Abort()
	{
		mDone = true;
		mMission->SetStatus(PathMission::PathStatus::None);
		ResetNavGrid();
	}

	void FindPathTask::Update()
	{
		if (mDone)
		{
			ResetNavGrid();
			return;
		}

		if (!mAlgorithm->IsExplorable())
		{
			mDone = true;
			mMission->SetStatus(PathMission::PathStatus::NotFound);
			ResetNavGrid();
			return;
		}

		mAlgorithm->ExploreNext();
		if (mAlgorithm->IsGoal())
		{
			mDone = true;
			mMission->SetStatus(PathMission::PathStatus::Found);
			ConstructPath();
			ResetNavGrid();
			return;
		}

		glm::ivec2 directions[]
		{
			{  0,  1 },
			{  0, -1 },
			{  1,  0 },
			{ -1,  0 },

			{  1,  1 },
			{  1, -1 },
			{ -1,  1 },
			{ -1,  -1 },
		};

		for (const glm::ivec2& direction : directions)
		{
			mAlgorithm->ExploreNeighbour(direction);
		}
	}

	bool FindPathTask::IsDone() const
	{
		return mDone;
	}

	void FindPathTask::ConstructPath()
	{
		Path& path = mMission->GetPath();
		path.Clear();
		for (uavpf::NavNode* node : mAlgorithm->ConstructPath())
		{
			glm::ivec2 navCoords = mNavGrid.GetCoordinates(node);
			path.AddCoordinate(navCoords, mNavGrid.GetElevation(navCoords) + mMission->GetMinElevation());
		}
		mPublisher.Publish<PathBuiltEvent>(EventPublishMode::Queued);
	}

	void FindPathTask::ResetNavGrid()
	{
		// EVIL HACK: set global costs grid to the global grid pointer,
		// otherwise it will try to access dangling pointer when the
		// current task dies. Algorithm sets nav grid in ctor, which is very
		// bad and craves for refactoring.
		mMission->GetCosts().SetGrid(&mMission->GetNavGrid());
	}

	bool FindPathTask::OnCancelPathFinding(const CancelPathFindingEvent& event)
	{
		Abort();
		return true;
	}
}

