#pragma once

#include "gtest/gtest.h"
#include "Task/MessageTask.hpp"
#include "Exception/Error.hpp"

class MessageTaskTest : public testing::Test {};

using namespace Framework::Task;


namespace MessageTaskUnitTest {
	enum class Commands : int {
		TEST_COMMAND_1 = 0,
		TEST_COMMAND_2 = 1,
	};
	// constexpr EventRequest::Command TEST_COMMAND_1 = 0;
	// constexpr EventRequest::Command TEST_COMMAND_2 = 1;

	EventRequest<Commands> lastRequest{ "", static_cast<Commands>(-1) };

	bool TestHandler(const MessageEventArgs<Commands> &args) {
		lastRequest = args.GetRequest();
		return true;
	}

	bool RpcTestHandler(const MessageEventArgs<Commands> &args) {
		lastRequest = args.GetRequest();
		std::this_thread::sleep_for(std::chrono::milliseconds(600));
		if (args.GetRequest().GetCommand() == Commands::TEST_COMMAND_1) {
			return true;
		}
		return false;
	}

	bool ThrowException(const MessageEventArgs<Commands> &args) {
		if (args.GetRequest().GetCommand() == Commands::TEST_COMMAND_1) {
			throw Framework::Exception("Command1Exception", Framework::Error::Code::Success);
		}
		throw Framework::Exception("Command2Exception", Framework::Error::Code::OutOfRange);
	}
}

const MessageTask<MessageTaskUnitTest::Commands>::EventMap events{
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_1, {MessageTaskUnitTest::TestHandler} },
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_2, {MessageTaskUnitTest::TestHandler} }
};

const MessageTask<MessageTaskUnitTest::Commands>::EventMap rpcEvents{
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_1, {MessageTaskUnitTest::RpcTestHandler} },
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_2, {MessageTaskUnitTest::RpcTestHandler} }
};

const MessageTask<MessageTaskUnitTest::Commands>::EventMap exceptionEvents{
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_1, {MessageTaskUnitTest::ThrowException} },
	{ MessageTaskUnitTest::Commands::TEST_COMMAND_2, {MessageTaskUnitTest::ThrowException} }
};

TEST_F(MessageTaskTest, StartStop) {
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", events);
	bool onStartCalled = false;
	bool onFinishCalled = false;

	task.SetOnStart([&onStartCalled]() {
		onStartCalled = true;
	});
	task.SetOnFinish([&onFinishCalled]() {
		onFinishCalled = true;
	});

	task.Start();
	EXPECT_TRUE(task.IsRunning());
	task.Stop();
	EXPECT_FALSE(task.IsRunning());

	EXPECT_TRUE(onStartCalled);
	EXPECT_TRUE(onFinishCalled);
}

TEST_F(MessageTaskTest, SendEventWithEventMapInitialization) {
	std::string command1Payload = "Command1";
	std::string command2Payload = "Command2";
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", events);

	task.Start();

	task.SendEvent({ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_1, command1Payload });
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_1, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command1Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());

	EventRequest request{ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_2, command2Payload };
	task.SendEvent(request);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_2, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command2Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());
}

TEST_F(MessageTaskTest, SendEventWithInitializerList) {
	std::string command1Payload = "Command1";
	std::string command2Payload = "Command2";
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", {
		{ MessageTaskUnitTest::Commands::TEST_COMMAND_1, {MessageTaskUnitTest::TestHandler} },
		{ MessageTaskUnitTest::Commands::TEST_COMMAND_2, {MessageTaskUnitTest::TestHandler} }
		});

	task.Start();

	task.SendEvent({ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_1, command1Payload });
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_1, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command1Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());

	EventRequest request{ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_2, command2Payload };
	task.SendEvent(request);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_2, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command2Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());
}

TEST_F(MessageTaskTest, RpcEvent) {
	std::string command1Payload = "Command1";
	std::string command2Payload = "Command2";
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", rpcEvents);

	task.Start();

	EXPECT_TRUE(task.RpcEvent({ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_1, command1Payload }));
	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_1, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command1Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());

	EventRequest request{ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_2, command2Payload };
	EXPECT_FALSE(task.RpcEvent(request));
	EXPECT_EQ("Test", MessageTaskUnitTest::lastRequest.GetFrom());
	EXPECT_EQ(MessageTaskUnitTest::Commands::TEST_COMMAND_2, MessageTaskUnitTest::lastRequest.GetCommand());
	EXPECT_STREQ(command2Payload.c_str(), MessageTaskUnitTest::lastRequest.GetPayloadAs<std::string>().c_str());
}

TEST_F(MessageTaskTest, RpcEventTimeout) {
	std::string command1Payload = "Command1";
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", rpcEvents);

	task.Start();

	EXPECT_FALSE(task.RpcEvent({ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_1, command1Payload }, std::chrono::milliseconds(500)));
}

TEST_F(MessageTaskTest, ExceptionInHandler) {
	std::string command1Payload = "Command1";
	std::string command2Payload = "Command2";
	MessageTask<MessageTaskUnitTest::Commands> task("TestTask", exceptionEvents);

	task.Start();

	Framework::Exception exception{ "", Framework::Error::Code::Unknown };

	try {
		task.RpcEvent({ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_1, command1Payload });
	} catch (const Framework::Exception &e) {
		exception = e;
	}
	EXPECT_EQ(static_cast<int>(Framework::Error::Code::Success), static_cast<int>(exception.GetCode()));

	try {
		EventRequest request{ "Test", MessageTaskUnitTest::Commands::TEST_COMMAND_2, command2Payload };
		task.RpcEvent(request);
	} catch (const Framework::Exception &e) {
		exception = e;
	}
	EXPECT_EQ(static_cast<int>(Framework::Error::Code::OutOfRange),
		static_cast<int>(exception.GetCode()));
}

// TEST_F(MessageTaskTest, SAFINE) {
// 	MessageEventAggregator::EventMap events1 {}
// }


