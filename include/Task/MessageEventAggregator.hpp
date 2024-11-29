#pragma once

#include <functional>
#include <map>
#include <initializer_list>
#include <type_traits>

#include "Exception/Exception.hpp"

#include "Task/EventRequest.hpp"
#include "Task/EventTaskBase.hpp"
#include "Task/interface/IEventAggregator.hpp"

namespace Framework::Task {
	using namespace Framework;

	template <typename T = EventRequest<>::Command,
		typename U = int64_t>
	class MessageEventAggregator :
		public EventTaskBase<T>::EventAggregator {
	public:
		using EventHandler = std::function<bool(const MessageEventArgs<T> &)>;
		using Command = T;
		using State = U;
		static constexpr State KEEP_STATE = static_cast<State>(-1);

		template<typename _T = State>
		class MessageInfo {
			EventHandler _handler;
			_T _nextState;
		public:
			MessageInfo(EventHandler handler, _T nextState = KEEP_STATE)
				: _handler(handler), _nextState(nextState) {}
			
			template <typename _U>
			MessageInfo(const MessageInfo<_U> &other) : _handler(other._handler) {
				if (std::is_same_v<_U, _T>) {
					_nextState = other._nextState;
				} else {
					_nextState = static_cast<_T>(other.GetNextState());
				}
			}

			template <typename _U>
			bool Handle(const MessageEventArgs<_U> &args) {
				return _handler(args);
			}

			_T GetNextState() const { return _nextState; }
		};

		using MessageEvent = std::pair<const Command, MessageInfo<State>>;
		using EventMap = std::map<Command, MessageInfo<State>>;
	private:
		EventMap _events;

	public:
		MessageEventAggregator(const EventMap &events) : _events(events) {}

		bool Publish(Command command, const MessageEventArgs<T> &args) override {
			try {
				auto &info = _events.at(command);
				return info.Handle(args);
			} catch (const std::out_of_range &e) {
				throw Exception("Event not found", Error::Code::OutOfRange);
			}
		}

		State GetNextState(Command command) const {
			try {
				return _events.at(command).GetNextState();
			} catch (const std::out_of_range &e) {
				throw Exception("Event not found", Error::Code::OutOfRange);
			}
		}
	};
} // namespace Framework::Task
