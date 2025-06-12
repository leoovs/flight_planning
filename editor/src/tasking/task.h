#pragma once

#include <cinttypes>

#include <vector>
#include <memory>
#include <stack>

namespace editor
{
	class Task
	{
	public:
		virtual ~Task() = default;

		virtual void Start() = 0;
		virtual void Abort() = 0;
		virtual void Update() = 0;

		virtual bool IsDone() const = 0;
	};

	class TaskCollection : public Task
	{
	public:
		virtual void Add(std::unique_ptr<Task> task) = 0;
	};

	class TaskGroup final : public TaskCollection
	{
	public:
		~TaskGroup() override = default;

		void Start() override;
		void Abort() override;
		void Update() override;

		bool IsDone() const override;

		void Add(std::unique_ptr<Task> task) override;

	private:
		std::vector<std::unique_ptr<Task>> mGroup;
	};

	class TaskSequence final : public TaskCollection
	{
	public:
		~TaskSequence() override = default;

		void Start() override;
		void Abort() override;
		void Update() override;

		bool IsDone() const override;

		void Add(std::unique_ptr<Task> task) override;

	private:
		std::vector<std::unique_ptr<Task>> mSequence;
	};

	class EmptyTask final : public Task
	{
	public:
		EmptyTask(int32_t idling = 1);
		~EmptyTask() override = default;

		void Start() override;
		void Abort() override;
		void Update() override;

		bool IsDone() const override;

	private:
		const int32_t mIdling = 1;
		int32_t mIdled = 0;
	};

	class TaskBuilder
	{
	public:
		TaskBuilder& BeginGroup();
		TaskBuilder& BeginSequence();
		TaskBuilder& End();

		std::unique_ptr<TaskCollection> Build();

		template<typename TaskT, typename... ArgsT>
		TaskBuilder& Add(ArgsT&&... args)
		{
			GetCurrentCollection()
				.Add(std::make_unique<TaskT>(std::forward<ArgsT>(args)...));
			return *this;
		}

	private:
		TaskCollection& GetCurrentCollection() const;

		std::unique_ptr<TaskCollection> mCurrentCollection;
		std::stack<std::unique_ptr<TaskCollection>> mTaskCollections;
	};
}

