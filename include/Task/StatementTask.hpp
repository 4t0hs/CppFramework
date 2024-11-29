#pragma once

#include "Task/EventTaskBase.hpp"
#include "Task/StateMachine.hpp"

#include "Templates/Property.hpp"

namespace Framework::Task {
	using namespace Framework::Templates;

	template <typename T = StateMachine<>::State,
		typename U = StateMachine<>::Command,
		std::enable_if_t<(std::is_integral_v<T> || std::is_enum_v<T>) &&
		(std::is_integral_v<U> || std::is_enum_v<U>), nullptr_t> = nullptr>
	class StatementTask : public EventTaskBase<U> {
	public:
		using State = T;
		using Command = U;

		using StateMachine = class StateMachine<Command, State>;
		using StateTable = typename StateMachine::StateTable;
		using StateEvents = typename StateMachine::StateEvents;

		using Events = typename StateMachine::EventAggregator;
		using EventHandler = typename Events::EventHandler;
		static constexpr State KEEP_STATE = Events::KEEP_STATE;
	private:
		using _Base = EventTaskBase<Command>;
		StateMachine _stateMachine;
	public:
		StatementTask(const std::string &name, const StateTable &table, State initialState)
			: _Base(TaskType::STATEMENT, name, &_stateMachine),
			_stateMachine(table, initialState) {}

		void SetState(State newState) {
			_stateMachine.SetState(newState);
		}

		State GetState() const {
			return _stateMachine.GetState();
		}

		ReferenceProperty::FunctionSetter<void(State, State)> stateChanged{ _stateMachine.stateChanged };
	};

} // namespace Framework::Task
