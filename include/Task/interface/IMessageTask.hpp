#pragma once

#include <chrono>

#include "Task/TaskBase.hpp"
#include "Task/EventRequest.hpp"

#include "Exception/Exception.hpp"

namespace Framework::Task {
	template <typename T = EventRequest<>::Command>
	class IEventTask {
	public:
		static constexpr auto WAIT_FOREVER = std::chrono::milliseconds::zero();

		// IEventTask(TaskType type, const std::string &name) : TaskBase(type, name) {}

		virtual void Start() = 0;
		virtual void Stop() = 0;

		virtual void SendEvent(EventRequest<T> &&message) = 0;
		virtual void SendEvent(const EventRequest<T> &message) = 0;

		virtual bool RpcEvent(EventRequest<T> &&message, std::chrono::milliseconds timeoutMsec = WAIT_FOREVER) = 0;
		virtual bool RpcEvent(const EventRequest<T> &message, std::chrono::milliseconds timeoutMsec = WAIT_FOREVER) = 0;
	};
} // namespace Framework::Task
