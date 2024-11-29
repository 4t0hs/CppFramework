#pragma once

#include "Exception/Error.hpp"

namespace Framework::Task {
	using namespace Framework;

	template <typename ...HandlerArgs>
	class IEventAggregator {
	public:
		virtual bool Publish(HandlerArgs...) = 0;
	};
} // namespace Framework::Task
