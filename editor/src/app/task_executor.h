#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace editor
{
	class TaskExecutor
	{
	public:
		using TaskFn = std::function<void()>;

		void Start();
		void Finish();

		void Execute(
			TaskFn fn,
			std::function<void()> onComplete);

	private:
		struct TaskHandle
		{
			TaskFn Fn;
			std::function<void()> OnComplete;
		};

		std::mutex mTaskQueueMutex;
		std::queue<TaskHandle> mTaskQueue;

		std::atomic_bool mWorkerRunning = false;
		std::thread mWorker;
	};
}

