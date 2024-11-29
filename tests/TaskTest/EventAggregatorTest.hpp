#pragma once

#include "gtest/gtest.h"
#include "Task/EventAggregator.hpp"

using namespace Framework::Task;

class EventAggregatorTest : public testing::Test {};

namespace EventAggregatorUnitTest {
	constexpr EventRequest::Command TEST_COMMAND_1 = 0;
	constexpr EventRequest::Command TEST_COMMAND_2 = 1;
}


TEST_F(EventAggregatorTest, InitializeWithEventMap) {
	std::string name;
	EventRequest::Command command;
	std::any payload;
	const MessageEventAggregator::EventMap events{
		{ EventAggregatorUnitTest::TEST_COMMAND_1, MessageEventAggregator::MessageInfo([&](const MessageEventArgs &args) {
				const EventRequest &request = args.GetRequest();
				name = request.GetFrom();
				command = request.GetCommand();
				payload = request.GetPayload();
				return true;
			})
		},
		{ EventAggregatorUnitTest::TEST_COMMAND_2, MessageEventAggregator::MessageInfo([&](const MessageEventArgs &args) {
				const EventRequest &request = args.GetRequest();
				name = request.GetFrom();
				command = request.GetCommand();
				payload = request.GetPayload();
				return false;
			})
		}
	};
	EventRequest command1Request("EventAggregatorTest", EventAggregatorUnitTest::TEST_COMMAND_1, std::string("Test"));
	EventRequest command2Request("EventAggregatorTest", EventAggregatorUnitTest::TEST_COMMAND_2, std::string("Test"));
	MessageEventAggregator aggregator(events);

	EXPECT_TRUE(aggregator.Publish(EventAggregatorUnitTest::TEST_COMMAND_1, MessageEventArgs(&command1Request)));
	EXPECT_STREQ("EventAggregatorTest", name.c_str());
	EXPECT_EQ(EventAggregatorUnitTest::TEST_COMMAND_1, command);
	EXPECT_STREQ("Test", std::any_cast<std::string>(payload).c_str());

	EXPECT_FALSE(aggregator.Publish(EventAggregatorUnitTest::TEST_COMMAND_2, MessageEventArgs(&command2Request)));
	EXPECT_STREQ("EventAggregatorTest", name.c_str());
	EXPECT_EQ(EventAggregatorUnitTest::TEST_COMMAND_2, command);
	EXPECT_STREQ("Test", std::any_cast<std::string>(payload).c_str());
}

TEST_F(EventAggregatorTest, InitializeWithInitializerList) {
	std::string name;
	EventRequest::Command command;
	std::any payload;
	const std::initializer_list<MessageEventAggregator::MessageEvent> events{
		{ EventAggregatorUnitTest::TEST_COMMAND_1, MessageEventAggregator::MessageInfo([&](const MessageEventArgs &args) {
				const EventRequest &request = args.GetRequest();
				name = request.GetFrom();
				command = request.GetCommand();
				payload = request.GetPayload();
				return true;
			})
		},
		{ EventAggregatorUnitTest::TEST_COMMAND_2, MessageEventAggregator::MessageInfo([&](const MessageEventArgs &args) {
				const EventRequest &request = args.GetRequest();
				name = request.GetFrom();
				command = request.GetCommand();
				payload = request.GetPayload();
				return false;
			})
		}
	};
	EventRequest command1Request("EventAggregatorTest", EventAggregatorUnitTest::TEST_COMMAND_1, std::string("Test"));
	EventRequest command2Request("EventAggregatorTest", EventAggregatorUnitTest::TEST_COMMAND_2, std::string("Test"));
	MessageEventAggregator aggregator(events);

	EXPECT_TRUE(aggregator.Publish(EventAggregatorUnitTest::TEST_COMMAND_1, MessageEventArgs(&command1Request)));
	EXPECT_STREQ("EventAggregatorTest", name.c_str());
	EXPECT_EQ(EventAggregatorUnitTest::TEST_COMMAND_1, command);
	EXPECT_STREQ("Test", std::any_cast<std::string>(payload).c_str());

	EXPECT_FALSE(aggregator.Publish(EventAggregatorUnitTest::TEST_COMMAND_2, MessageEventArgs(&command2Request)));
	EXPECT_STREQ("EventAggregatorTest", name.c_str());
	EXPECT_EQ(EventAggregatorUnitTest::TEST_COMMAND_2, command);
	EXPECT_STREQ("Test", std::any_cast<std::string>(payload).c_str());
}
