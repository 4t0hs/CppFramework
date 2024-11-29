#pragma once

#include <memory>
#include "IMessageQueue.hpp"
#include "SynchronizedDeque.hpp"

namespace Framework::Message {
	class MessageQueueFactory final {
	public:
		template<typename T>
		static IMessageQueue<T> *Create() {
			return new SynchronizedDeque<T>();
		}
	};
} // namespace Framework::Message
