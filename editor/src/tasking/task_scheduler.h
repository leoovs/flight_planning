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
		void Push(std::unique_ptr<Task> task);
		void Update();
		void Abort();

	private:
		void PeekNextTask();

		std::unique_ptr<Task> mCurrentTask;
		std::queue<std::unique_ptr<Task>> mTasks;
	};
}


