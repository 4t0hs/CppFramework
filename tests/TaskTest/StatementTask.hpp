#pragma once

#include "gtest/gtest.h"
#include "Task/StatementTask.hpp"

class StatementTaskTest : public ::testing::Test {};

using namespace Framework::Task;

namespace StatementTaskUnitTest {
	enum class TestState {
		IDLE, RUNNING, STOPPED
	};
	enum class TestCommand {
		START, STOP
	};

	EventRequest<TestCommand> lastRequest {"", static_cast<TestCommand>(-1)};
	bool eventHandler(const MessageEventArgs<TestCommand> &args) {
		lastRequest = args.GetRequest();
		return true;
	}

	StatementTask<TestState, TestCommand>::Events stoppedEvents {{
		{ TestCommand::START, { eventHandler, TestState::IDLE } },
		{ TestCommand::STOP, { eventHandler, StatementTask<TestState, TestCommand>::KEEP_STATE } }
	}};

	StatementTask<TestState, TestCommand>::Events idleEvents = {{
		{ TestCommand::START, { eventHandler, TestState::RUNNING } },
		{ TestCommand::STOP, { eventHandler, TestState::STOPPED } }
	}};

	StatementTask<TestState, TestCommand>::Events runningEvents = {{
		{ TestCommand::START, { eventHandler, StatementTask<TestState, TestCommand>::KEEP_STATE } },
		{ TestCommand::STOP, { eventHandler, TestState::IDLE } }
	}};

	StatementTask<TestState, TestCommand>::StateTable table {
		{ TestState::IDLE, idleEvents },
		{ TestState::RUNNING, runningEvents },
		{ TestState::STOPPED, stoppedEvents }
	};
} // namespace StatementUnitTest

using namespace StatementTaskUnitTest;

#include <iostream>

TEST_F(StatementTaskTest, NormalSequence) {
	StatementTask<TestState, TestCommand> task("test", table, TestState::STOPPED);
	EventRequest<TestCommand> startRequest {"", TestCommand::START};
	EventRequest<TestCommand> stopRequest {"", TestCommand::STOP};

	int actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);

	task.SendEvent(startRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::IDLE), actual);

	task.SendEvent(startRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::RUNNING), actual);

	task.SendEvent(stopRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::IDLE), actual);

	task.SendEvent(stopRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);
}

TEST_F(StatementTaskTest, RpcSequence) {
	StatementTask<TestState, TestCommand> task("test", table, TestState::STOPPED);
	EventRequest<TestCommand> startRequest {"", TestCommand::START};
	EventRequest<TestCommand> stopRequest {"", TestCommand::STOP};

	int actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);

	task.RpcEvent(startRequest);
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::IDLE), actual);

	task.RpcEvent(startRequest);
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::RUNNING), actual);

	task.RpcEvent(stopRequest);
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::IDLE), actual);

	task.RpcEvent(stopRequest);
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);
}

TEST_F(StatementTaskTest, NoStateTransition) {
	StatementTask<TestState, TestCommand> task("test", table, TestState::STOPPED);
	EventRequest<TestCommand> stopRequest {"", TestCommand::STOP};

	int actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);

	task.SendEvent(stopRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::STOPPED), actual);
}

TEST_F(StatementTaskTest, StateChangedHandler) {
	StatementTask<TestState, TestCommand> task("test", table, TestState::STOPPED);
	EventRequest<TestCommand> startRequest {"", TestCommand::START};
	EventRequest<TestCommand> stopRequest {"", TestCommand::STOP};
	int lastNewState = -1;
	int lastOldState = -1;

	task.stateChanged = [&](TestState oldState, TestState newState) {
		lastOldState = static_cast<int>(oldState);
		lastNewState = static_cast<int>(newState);
	};

	task.SendEvent(stopRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(-1, lastOldState);
	EXPECT_EQ(-1, lastNewState);

	task.SendEvent(startRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(static_cast<int>(TestState::STOPPED), lastOldState);
	EXPECT_EQ(static_cast<int>(TestState::IDLE), lastNewState);
}

TEST_F(StatementTaskTest, SetState) {
	StatementTask<TestState, TestCommand> task("test", table, TestState::STOPPED);
	EventRequest<TestCommand> startRequest {"", TestCommand::START};

	task.SetState(TestState::IDLE);
	int actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::IDLE), actual);

	task.SendEvent(startRequest);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	actual = static_cast<int>(task.GetState());
	EXPECT_EQ(static_cast<int>(TestState::RUNNING), actual);
}
