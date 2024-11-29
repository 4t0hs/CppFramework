#pragma once

#include "gtest/gtest.h"
#include "Task/MessageRequest.hpp"

class EventRequestTest : public ::testing::Test {};

using namespace Framework::Task;

TEST_F(EventRequestTest, FromAndCommand) {
	const std::string from = "Test";
	const EventRequest::Command command = 0;
	EventRequest request(from, command);

	EXPECT_STREQ(from.c_str(), request.GetFrom().c_str());
	EXPECT_EQ(command, request.GetCommand());
}

TEST_F(EventRequestTest, Payload) {
	const std::string input = "Test";
	EventRequest request("Test", 0, input);
	
	const std::any &payload = request.GetPayload();
	std::string data = std::any_cast<std::string>(payload);

	EXPECT_TRUE(request.HasPayload());
	EXPECT_STREQ(input.c_str(), request.GetPayloadAs<std::string>().c_str());
	EXPECT_STREQ(input.c_str(), data.c_str());

	EventRequest request2("Test", 0);
	EXPECT_FALSE(request2.HasPayload());
}

TEST_F(EventRequestTest, CopyAssignment) {
	const std::string from = "Test";
	const EventRequest::Command command = 0;
	const std::string input = "TestPayload";
	EventRequest request(from, command, input);

	EventRequest request2;
	request2 = request;

	EXPECT_STREQ(from.c_str(), request2.GetFrom().c_str());
	EXPECT_EQ(command, request2.GetCommand());
	EXPECT_TRUE(request2.HasPayload());
	EXPECT_STREQ(input.c_str(), request2.GetPayloadAs<std::string>().c_str());
}
