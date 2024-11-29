#include "gtest/gtest.h"
#include "Timer.h"

// NOLINTBEGIN

TEST(TimerTest, Oneshot) {
	Timer timer{ std::chrono::milliseconds(500) };
	int counter = 0;
	timer.AddListener([&] {
		counter++;
	});
	timer.Start();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	timer.Stop();
	EXPECT_EQ(1, counter);
}

TEST(TimerTest, Cycle) {
	Timer timer{ std::chrono::milliseconds(500) };
	int counter = 0;
	timer.AutoRestart(true);
	timer.AddListener([&] {
		counter++;
	});
	timer.Start();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	timer.Stop();
	EXPECT_GE(3, counter);
}

TEST(TimerTest, Listeners) {
	Timer timer{ std::chrono::milliseconds(500) };
	int counter{ 0 };
	timer.AddListener([&] {
		counter++;
	});
	timer.AddListener([&] {
		counter++;
	});
	timer.Start();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	timer.Stop();
	EXPECT_EQ(2, counter);
}

TEST(TimerTest, StartStop) {
	Timer timer {std::chrono::milliseconds(500)};
	int counter {0};
	timer.AddListener([&] {
		counter++;
	});
	timer.Start();
	std::this_thread::sleep_for(std::chrono::milliseconds(700));
	timer.Stop();
	timer.Start();
	std::this_thread::sleep_for(std::chrono::milliseconds(700));
	timer.Stop();
	EXPECT_EQ(2, counter);
}

// NOLINTEND
