#pragma once

#include <string_view>
#include <filesystem>
#include <thread>
#include "Main/Workspace.hpp"
#include "Main/Config.hpp"

namespace Framework::Task {

	enum class TaskType {
		UNKNOWN = 0,
		MESSAGE,
		STATEMENT,
		REAL_TIME,
		BACK_GROUND,
		TASK_POOL,
	};

	struct TaskInfomation {
		std::string name;
		TaskType type;
		std::thread::id threadId;
	};

	class TaskBase {
	protected:
		std::string _name;
		TaskType _type { TaskType::UNKNOWN };
		Main::Workspace _workspace;
		std::thread _thread;
	private:
		static std::filesystem::path _BuildWorkspacePath(const std::string &name) {
			std::filesystem::path path = Configuration::Address::Task();
			return std::move(path.append(name));
		}
	public:
		TaskBase(TaskType type, const std::string &name)
			: _name(name), _type{ type }, _workspace{ _BuildWorkspacePath(name) } {
			_workspace.Create();
		}

		~TaskBase() {
			_workspace.Remove();
		}

		virtual TaskInfomation GetTaskInfomation() {
			return { _name, _type, _thread.get_id() };
		}
	};
} // namespace Framework::Task
