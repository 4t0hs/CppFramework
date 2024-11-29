#pragma once

#include <map>
#include <chrono>
#include <cstddef>

namespace Framework::Message {
	template<class T, std::size_t T_SIZE = sizeof(T)>
	class IMessageQueue {
	public:
		virtual void Send(const T &message) = 0;
		virtual void Send(T &&message) = 0;
		virtual T Receive() = 0;
		virtual std::pair<bool, T> TimedReceive(const std::chrono::milliseconds milliSec) = 0;
		virtual bool IsEmpty() = 0;
		virtual void Clear() = 0;
		virtual std::size_t NumMessages() = 0;
	};
} // namespace Framework::Message
