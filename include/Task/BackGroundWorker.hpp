#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <any>

#include "Exception/Exception.hpp"

#include "Templates/Property.hpp"

#include "Task/TaskBase.hpp"


#include <iostream>
namespace Framework::Task {
	using namespace Framework::Templates;
	class BackGroundWorker : public TaskBase {
	public:
		class DoTaskEventArgs {
			bool _cancel{ false };
			std::any _result{};
		public:
			DoTaskEventArgs() = default;

			void SetCancel(bool cancel) { _cancel = cancel; }
			bool GetCancel() const { return _cancel; }

			void SetResult(const std::any &result) { _result = result; }
			void SetResult(std::any &&result) { _result = std::move(result); }
			const std::any &GetResult() const { return _result; }
		};

		class TaskCompletedEventArgs {
			bool _cancelled{ false };
			const std::any *_result{ nullptr };
			Framework::Error::Code _error{ Framework::Error::Code::Unknown };
		public:
			TaskCompletedEventArgs(bool cancelled, const std::any &result, Framework::Error::Code error)
				: _cancelled(cancelled), _result(&result), _error(error) {}

			bool Cancelled() const { return _cancelled; }
			Framework::Error::Code ErrorCode() const { return _error; }
			template <typename T = std::any>
			const auto &Result() const { return std::any_cast<const T &>(*_result); }
		};

		using Progress = uint8_t;

		class ProgressChangedEventArgs {
			Progress _progress{ 0 };
		public:
			explicit ProgressChangedEventArgs(Progress progress) : _progress(progress) {}
			Progress ProgressPercent() const { return _progress; }
		};


		using Task = std::function<void(BackGroundWorker &, DoTaskEventArgs &)>;
		using TaskCompletedEventHandler = std::function<void(BackGroundWorker &, TaskCompletedEventArgs &)>;
		using ProgressChangedEventHandler = std::function<void(BackGroundWorker &, ProgressChangedEventArgs &)>;
	private:
		std::thread _thread;
		Task _task;
		std::mutex _mutex;
		std::condition_variable _condition;
		bool _running{ false };
		bool _stop{ false };
		bool _cancellationPending{ false };
		Progress _progress{ 0 };

		TaskCompletedEventHandler _taskCompleted;
		ProgressChangedEventHandler _progressChanged;
	public:

		BackGroundWorker(const std::string &name, Task task) : TaskBase(TaskType::BACK_GROUND, name), _task(task) {
			_thread = std::thread([this] {
				while (true) {
					_Sleep();
					if (_stop) {
						break;
					}
					_OnDoTask();
				}
			});
		}

		virtual ~BackGroundWorker() {
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_stop = true;
				_condition.notify_all();
			}
			if (_thread.joinable()) {
				_thread.join();
			}
		}

		void RunTaskAsync() {
			std::lock_guard<std::mutex> lock(_mutex);
			_running = true;
			_condition.notify_all();
		}

		bool CancellationPending() {
			return _cancellationPending;
		}

		void CancelAsync() {
			_cancellationPending = true;
		}

		void ReportsProgress(Progress percent) {
			ProgressChangedEventArgs args(percent);
			if (_progressChanged) {
				_progressChanged(*this, args);
			}
		}

		bool IsBusy() {
			return _running;
		}

		ReferenceProperty::FunctionSetter<TaskCompletedEventHandler> TaskCompleted{ _taskCompleted };
		ReferenceProperty::FunctionSetter<ProgressChangedEventHandler> ProgressChanged{ _progressChanged };
	private:
		void _Sleep() {
			std::unique_lock<std::mutex> lock(_mutex);
			_condition.wait(lock, [this] {
				return _running || _stop;
			});
		}

		void _OnDoTask() {
			DoTaskEventArgs args;
			Framework::Error::Code error = Framework::Error::Code::Success;
			try {
				_task(*this, args);
			} catch (const Framework::Exception &e) {
				error = e.GetCode();
			}
			_OnTaskCompleted(args, error);
		}

		void _OnTaskCompleted(DoTaskEventArgs &doTaskEventArgs, Framework::Error::Code error) {
			if (!_taskCompleted) {
				return;
			}
			TaskCompletedEventArgs args{
				doTaskEventArgs.GetCancel(),
				doTaskEventArgs.GetResult(),
				error
			};
			_taskCompleted(*this, args);

			_cancellationPending = false;
			_running = false;
			_progress = 0;
		}
	};
} // namespace Framework::Task
