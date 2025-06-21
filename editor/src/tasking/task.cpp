#include "tasking/task.h"

#include <cassert>
#include <future>

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// TaskGroup
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void TaskGroup::Start()
	{
		for (const std::unique_ptr<Task>& task : mGroup)
		{
			task->Start();
		}
	}

	void TaskGroup::Abort()
	{
		for (const std::unique_ptr<Task>& task : mGroup)
		{
			task->Abort();
		}
	}

	void TaskGroup::Update()
	{
		for (const std::unique_ptr<Task>& task : mGroup)
		{
			task->Update();
		}
	}

	bool TaskGroup::IsDone() const
	{
		bool allDone = true;

		for (const std::unique_ptr<Task>& task : mGroup)
		{
			allDone = allDone && task->IsDone();
		}
		
		return allDone;
	}

	void TaskGroup::Add(std::unique_ptr<Task> task)
	{
		mGroup.push_back(std::move(task));
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// TaskSequence 
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void TaskSequence::Start()
	{
		PopNextTask();
	}

	void TaskSequence::Abort()
	{
		if (nullptr != mCurrent)
		{
			mCurrent->Abort();
			mCurrent.reset();
		}
		mSequence = {};
	}

	void TaskSequence::Update()
	{
		if (nullptr == mCurrent)
		{
			return;
		}

		mCurrent->Update();
		if (mCurrent->IsDone())
		{
			mCurrent = nullptr;
			PopNextTask();
		}
	}

	bool TaskSequence::IsDone() const
	{
		return nullptr == mCurrent;
	}

	void TaskSequence::PopNextTask()
	{
		if (mSequence.empty())
		{
			return;
		}

		mCurrent = std::move(mSequence.front());
		mSequence.pop();
		mCurrent->Start();
	}

	void TaskSequence::Add(std::unique_ptr<Task> task)
	{
		mSequence.push(std::move(task));
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// EmptyTask	
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	EmptyTask::EmptyTask(int32_t idling)
		: mIdling(idling)
	{}

	void EmptyTask::Start()
	{
		mIdled = 0;
	}

	void EmptyTask::Abort()
	{
		mIdled = mIdling;
	}

	void EmptyTask::Update()
	{
		mIdled++;
	}

	bool EmptyTask::IsDone() const
	{
		return mIdled >= mIdling;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// CpuBoundTask
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	CpuBoundTask::CpuBoundTask(std::function<void()> taskFunc)
		: mTaskFunc(std::move(taskFunc))
	{}

	CpuBoundTask::~CpuBoundTask() = default;

	void CpuBoundTask::Start()
	{
		mTaskFuture = std::async(std::launch::async, mTaskFunc);
	}

	void CpuBoundTask::Abort()
	{
		mTaskFuture.wait();
	}

	void CpuBoundTask::Update()
	{}

	bool CpuBoundTask::IsDone() const
	{
		return mTaskFuture.wait_for(std::chrono::seconds(0))
			== std::future_status::ready;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// SingleStepTask
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	SingleStepTask::SingleStepTask(std::function<void()> step)
		: mStep(std::move(step))
	{}

	void SingleStepTask::Start()
	{
		assert(nullptr != mStep);
	}

	void SingleStepTask::Abort()
	{
		mStep = nullptr;
	}

	void SingleStepTask::Update()
	{
		if (mStep)
		{
			mStep();
			mStep = nullptr;
		}
	}

	bool SingleStepTask::IsDone() const
	{
		return nullptr == mStep;
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	//	TaskBuilder
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	TaskBuilder& TaskBuilder::BeginGroup()
	{
		mTaskCollections.push(std::make_unique<TaskGroup>());
		return *this;
	}

	TaskBuilder& TaskBuilder::BeginSequence()
	{
		mTaskCollections.push(std::make_unique<TaskSequence>());
		return *this;
	}

	TaskBuilder& TaskBuilder::End()
	{
		assert(!mTaskCollections.empty() && "Missing BeginGroup/Sequence?");

		std::unique_ptr<TaskCollection> current = std::move(mTaskCollections.top());
		mTaskCollections.pop();

		bool currentHasParent = !mTaskCollections.empty();
		if (currentHasParent)
		{
			GetCurrentCollection().Add(std::move(current));
		}

		mCurrentCollection = std::move(current);

		return *this;
	}

	TaskBuilder& TaskBuilder::Do(std::function<void()> step)
	{
		return Add<SingleStepTask>(std::move(step));
	}

	TaskBuilder& TaskBuilder::DoThreaded(std::function<void()> job)
	{
		return Add<CpuBoundTask>(std::move(job));
	}

	std::unique_ptr<TaskCollection> TaskBuilder::Build()
	{
		std::unique_ptr<TaskCollection> current = std::move(mCurrentCollection);
		return current;
	}

	TaskCollection& TaskBuilder::GetCurrentCollection() const
	{
		assert(!mTaskCollections.empty() && "Missing BeginGroup/Sequence?");
		return *mTaskCollections.top();
	}
}
