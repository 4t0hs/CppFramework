#include "gtest/gtest.h"
#include "Sync/EventFlag.hpp"
#include <thread>
// NOLINTBEGIN
class EventFlagTest :public::testing::Test {
protected:
	EFlag::EventFlag eventFlag;
};

TEST_F(EventFlagTest, GetSet) {
	const EFlag::Flag flag {0x01};
	eventFlag.Set(flag);
	EXPECT_EQ(flag, eventFlag.Get());
}

TEST_F(EventFlagTest, And) {
	bool exited = false;
	std::thread t1{ [&] {
		eventFlag.Wait(0x01, EFlag::MatchMode::AND);
		exited = true;
	} };
	eventFlag.Set(0x01);
	t1.join();
	EXPECT_EQ(true, exited);
}

TEST_F(EventFlagTest, Or) {
	bool exited = false;
	std::thread t{[&] {
		eventFlag.Wait(0x11, EFlag::MatchMode::OR);
		exited = true;
	}};
	eventFlag.Set(0x01);
	t.join();
	EXPECT_EQ(true, exited);
}

TEST_F(EventFlagTest, TimedOut_And) {
	bool ret = false;
	std::thread t {[&] {
		ret = this->eventFlag.TimedWait(0x11, EFlag::MatchMode::AND, std::chrono::seconds(1));
	}};
	eventFlag.Set(0x01);
	t.join();
	EXPECT_EQ(false, ret);
}

TEST_F(EventFlagTest, TimedOut_Or) {
	bool ret = false;
	std::thread t {[&] {
		ret = this->eventFlag.TimedWait(0x02, EFlag::MatchMode::OR, std::chrono::seconds(1));
	}};
	eventFlag.Set(0x01);
	t.join();
	EXPECT_EQ(false, ret);
}

TEST_F(EventFlagTest, WaitMultiple_1) {
	int exited = 0;
	std::function<void()> f {[&] {
		if (this->eventFlag.TimedWait(0x01, EFlag::MatchMode::AND, std::chrono::seconds(1))) {
			exited++;
		}
	}};
	std::thread thread1{f}, thread2 {f};
	eventFlag.Set(0x01);
	thread1.join();
	thread2.join();
	EXPECT_EQ(2, exited);
}

TEST_F(EventFlagTest, WaitMultiple_2) {
	int exited = 0;
	std::thread thread1 {[&] {
		if (this->eventFlag.TimedWait(0x01, EFlag::MatchMode::AND, std::chrono::seconds(1))) {
			exited++;
		}
	}};
	std::thread thread2 {[&] {
		if (this->eventFlag.TimedWait(0x02, EFlag::MatchMode::AND, std::chrono::seconds(1))) {
			exited++;
		}
	}};
	eventFlag.Set(0x01);
	thread1.join();
	thread2.join();
	EXPECT_EQ(1, exited);
}

// NOLINTEND
