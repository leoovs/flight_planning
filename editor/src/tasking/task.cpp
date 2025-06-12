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
		mSequence.front()->Start();
	}

	void TaskSequence::Abort()
	{
		for (const std::unique_ptr<Task>& task : mSequence)
		{
			if (!task->IsDone())
			{
				task->Abort();
				continue;
			}
		}
	}

	void TaskSequence::Update()
	{
		for (const std::unique_ptr<Task>& task : mSequence)
		{
			if (!task->IsDone())
			{
				task->Update();
				return;
			}
		}
	}

	bool TaskSequence::IsDone() const
	{
		return mSequence.back()->IsDone();
	}

	void TaskSequence::Add(std::unique_ptr<Task> task)
	{
		mSequence.push_back(std::move(task));
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
