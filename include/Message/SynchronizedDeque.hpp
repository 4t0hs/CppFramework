#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include "IMessageQueue.hpp"

namespace Framework::Message {
	template<typename T>
	class SynchronizedDeque : public IMessageQueue<T> {
		static constexpr auto WAIT_FOREVER = std::chrono::milliseconds::zero();
		std::mutex _mutex;
		std::condition_variable _condition;
		std::deque<T> _queue{};

		inline T _GetFront() {
			T buffer = std::move(_queue.front());
			_queue.pop_front();
			return buffer;
		}

		inline bool IsNotEmpty() const {
			return !_queue.empty();
		}

	public:
		SynchronizedDeque() = default;

		void Send(const T &message) override {
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push_back(message);
			_condition.notify_all();
		}

		void Send(T &&message) override {
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push_back(std::move(message));
			_condition.notify_all();
		}

		T Receive() override {
			std::unique_lock<std::mutex> lock(_mutex);
			_condition.wait(lock, [this] { return IsNotEmpty(); });
			return _GetFront();
		}

		std::pair<bool, T> TimedReceive(const std::chrono::milliseconds milliSeconds) override {
			std::unique_lock<std::mutex> lock(_mutex);
			if (_condition.wait_for(lock, milliSeconds, [this] { return IsNotEmpty(); })) {
				return { true, _GetFront() };
			}
			return { false, T{} };
		}

		void Clear() override {
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.clear();
		}

		bool IsEmpty() override {
			std::lock_guard<std::mutex> lock(_mutex);
			return _queue.empty();
		}

		std::size_t NumMessages() override {
			std::lock_guard<std::mutex> lock(_mutex);
			return _queue.size();
		}
	};
} // namespace Framework::Message
