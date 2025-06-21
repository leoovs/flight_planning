#include "tasking/task_scheduler.h"

namespace editor
{
	void TaskScheduler::Push(std::unique_ptr<Task> task)
	{
		mTasks.push(std::move(task));
	}

	void TaskScheduler::Update()
	{
		if (nullptr == mCurrentTask)
		{
			if (mTasks.empty())
			{
				return;
			}
			PeekNextTask();
		}
		if (mCurrentTask->IsDone())
		{
			mCurrentTask = nullptr;
			return;
		}
		mCurrentTask->Update();
	}

	void TaskScheduler::Abort()
	{
		if (nullptr != mCurrentTask)
		{
			mCurrentTask->Abort();
			mCurrentTask = nullptr;
		}
	}

	void TaskScheduler::PeekNextTask()
	{
		mCurrentTask = std::move(mTasks.front());
		mTasks.pop();
		mCurrentTask->Start();
	}
}

