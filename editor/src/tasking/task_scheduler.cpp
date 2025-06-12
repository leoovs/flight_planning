#include "tasking/task_scheduler.h"

namespace editor
{
	void TaskScheduler::Push(std::unique_ptr<Task> task, TaskCompletionHandler onComplete)
	{
		mTasks.push(std::move(task));
		GetCurrentTask().Start();
		mOnComplete.push(std::move(onComplete));
	}

	void TaskScheduler::Update()
	{
		if (mTasks.empty())
		{
			return;
		}

		Task& current = GetCurrentTask();
		if (current.IsDone())
		{
			if (mOnComplete.front())
			{
				mOnComplete.front()(current);
			}
			mTasks.pop();
			return;
		}
		current.Update();
	}

	void TaskScheduler::Abort()
	{
		if (!mTasks.empty())
		{
			GetCurrentTask().Abort();
		}
	}

	Task& TaskScheduler::GetCurrentTask() const
	{
		return *mTasks.front();
	}
}

