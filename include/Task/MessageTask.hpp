#pragma once

#include <string>
#include <initializer_list>
#include <type_traits>

#include "Task/EventTaskBase.hpp"
#include "Task/MessageEventAggregator.hpp"

namespace Framework::Task {

	template <typename _CommandType = MessageEventAggregator<>::Command,
		std::enable_if_t<std::is_integral_v<_CommandType> || std::is_enum_v<_CommandType>,
		nullptr_t> = nullptr>
	class MessageTask : public EventTaskBase<_CommandType> {
		using _Base = EventTaskBase<_CommandType>;
	public:
		using EventAggregator = MessageEventAggregator<_CommandType>;
		using EventMap = EventAggregator::EventMap;
	private:
		EventAggregator _eventAggregator;

		// static constexpr EventAggregator::EventMap _Convert(const EventMap &events) {
		// 	EventAggregator::EventMap result;
		// 	for (const auto &event : events) {
		// 		result.insert({
		// 			static_cast<EventAggregator::Command>(event.first),
		// 			EventAggregator::MessageInfo<>(event.second) });
		// 	}
		// 	return result;
		// }
	public:
		// MessageTask(const std::string &name, const EventAggregator::EventMap &events) :
		// 	_Base(TaskType::MESSAGE, name, &_eventAggregator), _eventAggregator(events) {}

		MessageTask(const std::string &name, const EventMap &events) :
			_Base(TaskType::MESSAGE, name, &_eventAggregator), _eventAggregator(events) {}
	};
} // namespace Framework::Task
