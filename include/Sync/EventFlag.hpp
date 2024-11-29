#pragma once

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

namespace EFlag {
	using Flag = std::uint64_t;
	enum class MatchMode : std::uint8_t {
		AND, OR,
	};
	class EventFlag {
		std::atomic<Flag> _pattern;
		std::mutex _mutex;
		std::condition_variable _condition;
		[[nodiscard]] inline bool _Match(Flag pattern, MatchMode mode) {
			if (mode == MatchMode::AND) {
				return pattern == _pattern;
			}
			return pattern & _pattern;
		}
	public:
		void Wait(Flag pattern, MatchMode mode) {
			std::unique_lock<std::mutex> lock(_mutex);
			_condition.wait(lock, [&] {
				return this->_Match(pattern, mode);
			});
		}
		bool TimedWait(Flag pattern, MatchMode mode, std::chrono::milliseconds millisec) {
			std::unique_lock<std::mutex> lock(_mutex);
			return _condition.wait_for(lock, millisec, [&] {
				return this->_Match(pattern, mode);
			});
		}
		void Set(Flag pattern) {
			_pattern |= pattern;
		}
		Flag Get() {
			return _pattern;
		}
	};
}  // namespace EFlag
