#pragma once

#include <atomic>

#include "gtest/gtest.h"
#include "Task/TaskPool.hpp"

class TaskPoolTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};

namespace TaskPoolUnitTest {
	void SetAffinity(int n) {
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);
		for (int i = 0; i < n; i++) {
			CPU_SET(i, &cpu_set);
		}
		sched_setaffinity(0, sizeof(cpu_set), &cpu_set);
	}
}

using namespace Framework::Task;

TEST_F(TaskPoolTest, Constructor) {
	int numCpu = 5;
	TaskPoolUnitTest::SetAffinity(numCpu);
	TaskPool pool1{ "Test" };
	EXPECT_EQ(numCpu, pool1.Concurrency());
	TaskPool pool2{ "Test", 2 };
	EXPECT_EQ(2, pool2.Concurrency());
}

TEST_F(TaskPoolTest, Enqueue) {
	TaskPool pool{ "Test" };
	std::atomic_int counter = 0;
	for (int i = 0; i < 10; i++) {
		pool.Enqueue([&counter] {
			counter++;
		});
	}
	pool.Stop();
	EXPECT_EQ(10, counter);
}

TEST_F(TaskPoolTest, CountRunningTasks) {
	TaskPool pool{ "Test", 1 };
	bool paused = true;
	int numTasks = 10;

	for (int i = 0; i < numTasks; i++) {
		pool.Enqueue([&paused] {
			while (paused) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_EQ(1, pool.CountRunningTasks());
	paused = false;
}

TEST_F(TaskPoolTest, CountWaitingTasks) {
	TaskPool pool{ "Test", 1 };
	EXPECT_EQ(0, pool.CountWaitingTasks());
	pool.Enqueue([] {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	});
	pool.Enqueue([] {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_EQ(1, pool.CountWaitingTasks());
	pool.Stop();
	EXPECT_EQ(0, pool.CountWaitingTasks());
}

TEST_F(TaskPoolTest, IsEmpty) {
	TaskPool pool{ "Test" };
	EXPECT_TRUE(pool.IsEmpty());
	pool.Enqueue([] {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_TRUE(pool.IsEmpty());
	pool.Stop();
	EXPECT_TRUE(pool.IsEmpty());
}

TEST_F(TaskPoolTest, ClearWaitingTasks) {
	TaskPool pool{ "Test", 1 };
	EXPECT_TRUE(pool.IsEmpty());
	for (int i = 0; i < 10; i++) {
		pool.Enqueue([] {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		});
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	pool.ClearWaitingTasks();
	EXPECT_TRUE(pool.IsEmpty());
}
