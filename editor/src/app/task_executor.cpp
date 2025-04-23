#include "app/task_executor.h"

namespace editor
{
	void TaskExecutor::Start()
	{
		mWorkerRunning = true;
		auto taskJob = [this]()
			{
				while (mWorkerRunning)
				{
					if (mTaskQueue.empty())
					{
						std::this_thread::yield();
						continue;
					}

					mTaskQueueMutex.lock();
					TaskHandle handle = std::move(mTaskQueue.front());
					mTaskQueue.pop();
					mTaskQueueMutex.unlock();

					if (handle.Fn)
					{
						handle.Fn();
					}
					if (handle.OnComplete)
					{
						handle.OnComplete();
					}
				}
			};

		mWorker = std::thread(taskJob);
	}

	void TaskExecutor::Finish()
	{
		mWorkerRunning = false;
		mWorker.join();
	}

	void TaskExecutor::Execute(TaskFn fn, std::function<void()> onComplete)
	{
		mTaskQueueMutex.lock();
		mTaskQueue.push(TaskHandle{ std::move(fn), std::move(onComplete) });
		mTaskQueueMutex.unlock();
	}
}

