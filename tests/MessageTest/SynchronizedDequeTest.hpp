#pragma once

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "Message/SynchronizedDeque.hpp"

using namespace Framework::Message;

class SynchronizedDequeTest : public ::testing::Test {
protected:
	SynchronizedDeque<int> queue;
};

TEST_F(SynchronizedDequeTest, SendAndReceive) {
	queue.Send(42);
	EXPECT_EQ(42, queue.Receive());
}

TEST_F(SynchronizedDequeTest, SendAndReceiveMultiple) {
	queue.Send(1);
	queue.Send(2);
	queue.Send(3);
	EXPECT_EQ(1, queue.Receive());
	EXPECT_EQ(2, queue.Receive());
	EXPECT_EQ(3, queue.Receive());
}

TEST_F(SynchronizedDequeTest, TimedReceiveSuccess) {
	queue.Send(42);
	auto result = queue.TimedReceive(std::chrono::milliseconds(100));
	EXPECT_TRUE(result.first);
	EXPECT_EQ(42, result.second);
}

TEST_F(SynchronizedDequeTest, TimedReceiveTimeout) {
	auto result = queue.TimedReceive(std::chrono::milliseconds(100));
	EXPECT_FALSE(result.first);
}

TEST_F(SynchronizedDequeTest, ClearQueue) {
	queue.Send(1);
	queue.Send(2);
	queue.Clear();
	EXPECT_TRUE(queue.IsEmpty());
}

TEST_F(SynchronizedDequeTest, IsEmpty) {
	EXPECT_TRUE(queue.IsEmpty());
	queue.Send(42);
	EXPECT_FALSE(queue.IsEmpty());
}

TEST_F(SynchronizedDequeTest, NumMessages) {
	EXPECT_EQ(0, queue.NumMessages());
	queue.Send(1);
	queue.Send(2);
	EXPECT_EQ(2, queue.NumMessages());
	queue.Receive();
	EXPECT_EQ(1, queue.NumMessages());
}

TEST_F(SynchronizedDequeTest, ConcurrentSendAndReceive) {
	std::thread sender([&]() {
		for (int i = 0; i < 10; ++i) {
			queue.Send(i);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	std::thread receiver([&]() {
		for (int i = 0; i < 10; ++i) {
			int value = queue.Receive();
			EXPECT_EQ(i, value);
		}
	});

	sender.join();
	receiver.join();
}
