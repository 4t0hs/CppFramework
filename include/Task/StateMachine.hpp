#pragma once

#include <map>
#include <initializer_list>
#include <atomic>
#include <type_traits>
#include <functional>

#include "Task/EventTaskBase.hpp"
#include "Task/MessageEventAggregator.hpp"

#include "Templates/Property.hpp"

namespace Framework::Task {

	template <typename T = EventRequest<>::Command,
		typename U = MessageEventAggregator<>::State>
	class StateMachine : public EventTaskBase<T>::EventAggregator {
	public:
		using Command = T;
		using State = U;
		using EventAggregator = MessageEventAggregator<Command, State>;
		using StateTable =
			std::map<State, EventAggregator>;
		using StateEvents =
			std::pair<const State, EventAggregator>;

	private:
		static constexpr State UNDEFINED_STATE = static_cast<State>(-1);
		class StateInfo {
		public:
			State state { UNDEFINED_STATE };
			EventAggregator *aggregator { nullptr };
			StateInfo() = default;
			StateInfo(State state, EventAggregator &aggregator) : state(state), aggregator(&aggregator) {}
		};
		StateTable _table;
		std::atomic<StateInfo> _current;
	public:
		StateMachine(const StateTable &table, State initialState) : _table(table) {
			SetState(initialState);
		}

		void SetState(State newState) {
			State currentState = _current.load().state;
			if (currentState == newState) {
				return;
			}
			try {
				_current.store({ newState, _table.at(newState) });
				if (stateChanged) {
					stateChanged(currentState, newState);
				}
			} catch (const std::out_of_range &e) {
				throw Exception("State not found", Error::Code::OutOfRange);
			}
		}

		State GetState() const { return _current.load().state; }

		bool Publish(EventRequest<Command>::Command command, const MessageEventArgs<Command> &args) override {
			auto current = _current.load();
			const bool returnValue = current.aggregator->Publish(command, args);
			State nextState = current.aggregator->GetNextState(command);
			if ((nextState != MessageEventAggregator<Command, State>::KEEP_STATE) && returnValue) {
				SetState(nextState);
			}
			return returnValue;
		}
		std::function<void(State, State)> stateChanged;
	};

} // namespace Framework::Task
