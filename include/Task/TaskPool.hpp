#pragma once
#include <sched.h>
#include <unistd.h>
#ifndef __GNU_SOURCE
#define __GNU_SOURCE
#include <pthread.h>
#undef __GNU_SOURCE
#endif
#include <functional>
#include <vector>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "Task/TaskBase.hpp"

namespace Framework::Task {

	class TaskPool : public TaskBase {
		using Task = std::function<void()>;
	private:	
		std::deque<std::function<void()>> _tasks{};
		std::condition_variable _condition{};
		std::mutex _mutex{};
		std::vector<std::thread> _workers{};
		bool _stop {false};
		size_t _concurrency {0};
		std::atomic<size_t> _runningTasks {0};

	public:
		TaskPool(const std::string &name, size_t concurrency = TaskPool::_GetConcurrency())
			: TaskBase(TaskType::TASK_POOL, name), _concurrency{concurrency} {
			_SpawnWorkers();
		}

		~TaskPool() {
			Stop();
		}

		void Enqueue(Task task) {
			std::lock_guard<std::mutex> lock(_mutex);
			_tasks.emplace_back(std::move(task));
			_condition.notify_all();
		}

		void Stop() {
			_stop = true;
			_condition.notify_all();
			for (auto &worker : _workers) {
				if (worker.joinable()) {
					worker.join();
				}
			}
			_workers.clear();
		}

		size_t Concurrency() const {
			return _concurrency;
		}

		size_t CountWaitingTasks() {
			std::lock_guard<std::mutex> lock(_mutex);
			return _tasks.size();
		}

		size_t CountRunningTasks() {
			return _runningTasks;
		}

		void ClearWaitingTasks() {
			std::lock_guard<std::mutex> lock(_mutex);
			_tasks.clear();
		}

		bool IsEmpty() {
			std::lock_guard<std::mutex> lock(_mutex);
			return _tasks.empty();
		}

	private:
		void _SpawnWorkers() {
			for (size_t i = 0; i < _concurrency; i++) {
				_workers.emplace_back(std::thread {[this] {
					while (true) {
						auto task =_WaitForNewTask();
						if (_stop && !task) {
							return;
						}
						if (task) {
							_runningTasks++;
							task();
							_runningTasks--;
						}
					}
				}});
			}
		}

		Task _WaitForNewTask() {
			std::unique_lock<std::mutex> lock(_mutex);
			_condition.wait(lock, [this] {
				return !_tasks.empty() || _stop;
			});
			if (_tasks.empty()) {
				return {};
			}
			auto task = std::move(_tasks.front());
			_tasks.pop_front();
			return task;
		}

		static size_t _GetConcurrency() {
			cpu_set_t cpu_set {0};
			CPU_ZERO(&cpu_set);
			pthread_getaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);
			return static_cast<size_t>(CPU_COUNT(&cpu_set));
		}
	};
} // namespace Framework::Task
