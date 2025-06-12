#pragma once

#include <functional>
#include <queue>

#include "tasking/task.h"

namespace editor
{
	using TaskCompletionHandler = std::function<void(Task&)>;

	class TaskScheduler
	{
	public:
		void Push(std::unique_ptr<Task> task, TaskCompletionHandler onComplete);
		void Update();
		void Abort();

	private:
		Task& GetCurrentTask() const;

		std::queue<std::unique_ptr<Task>> mTasks;
		std::queue<TaskCompletionHandler> mOnComplete;
	};
}


