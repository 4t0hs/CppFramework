#pragma once

#include <thread>
#include <functional>
#include <chrono>
#include <atomic>
#include <vector>
#include <iostream>

class Timer {
	std::chrono::milliseconds _time;
	std::atomic<bool> _restart{ false };
	std::atomic<bool> _active{ false };
	std::thread _thread;
	std::vector<std::function<void()>> _eventHandlers{};

public:
	explicit Timer(std::chrono::milliseconds time) : _time(time) {}
	~Timer() {
		_WaitForStop();
	}

	void AutoRestart(bool val) {
		_restart = val;
	}

	void AddListener(const std::function<void()> &f) {
		_eventHandlers.emplace_back(f);
	}

	void Start() {
		if (_active) {
			return;
		}
		if (_thread.joinable()) {
			_thread.join();
		}
		_active = true;
		_thread = std::move(std::thread{ [&] {
			std::vector<std::function<void()>> handlers {this->_eventHandlers};
			do {
				std::this_thread::sleep_for(this->_time);
				if (!this->_active) {
					break;
				}
				for (const auto &f : handlers) {
					std::invoke(f);
				}
			} while (this->_restart);
			this->_thread.detach();
		} });
	}

	void Stop() {
		if (_active)
			_active = false;
	}

	void _WaitForStop() {
		Stop();
		if (_thread.joinable()) {
			_thread.join();
		}
	}
};
