#pragma once

#include <thread>

#include "gtest/gtest.h"

#include "Task/BackGroundWorker.hpp"

class BackGroundWorkerTest : public ::testing::Test {};

using namespace Framework::Task;

TEST_F(BackGroundWorkerTest, ConstructorAndDestructor) {
	BackGroundWorker worker {"test", [](BackGroundWorker &, BackGroundWorker::DoTaskEventArgs &){}};
}

TEST_F(BackGroundWorkerTest, SimpleTask) {
	BackGroundWorker worker {"test", [&](BackGroundWorker &, BackGroundWorker::DoTaskEventArgs &e){
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		e.SetResult(true);
	}};
	bool taskResult = false;
	bool completed = false;
	worker.TaskCompleted = [&](BackGroundWorker &, BackGroundWorker::TaskCompletedEventArgs &e) {
		taskResult = e.Result<bool>();
		completed = true;
	};

	worker.RunTaskAsync();
	
	EXPECT_TRUE(worker.IsBusy());

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	EXPECT_TRUE(taskResult);
	EXPECT_TRUE(completed);
}

TEST_F(BackGroundWorkerTest, ReportingProgress) {
	BackGroundWorker worker {"test", [](BackGroundWorker &worker, BackGroundWorker::DoTaskEventArgs &) {
		worker.ReportsProgress(50);
	}};
	BackGroundWorker::Progress progress;
	worker.ProgressChanged = [&](BackGroundWorker &, BackGroundWorker::ProgressChangedEventArgs &e) {
		progress = e.ProgressPercent();
	};

	worker.RunTaskAsync();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(50, progress);
}

TEST_F(BackGroundWorkerTest, Cancellation) {
	BackGroundWorker worker {"test", [](BackGroundWorker &worker, BackGroundWorker::DoTaskEventArgs &e) {
		worker.ReportsProgress(10);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if (worker.CancellationPending()) {
			e.SetCancel(true);
			return;
		}
		worker.ReportsProgress(100);
	}};
	BackGroundWorker::Progress latestProgress;
	worker.ProgressChanged = [&](BackGroundWorker &, BackGroundWorker::ProgressChangedEventArgs &e) {
		latestProgress = e.ProgressPercent();
	};
	bool cancelled = false;
	worker.TaskCompleted = [&](BackGroundWorker &, BackGroundWorker::TaskCompletedEventArgs &e) {
		cancelled = e.Cancelled();
	};

	worker.RunTaskAsync();
	worker.CancelAsync();

	std::this_thread::sleep_for(std::chrono::milliseconds(1100));

	EXPECT_TRUE(cancelled);
	EXPECT_EQ(10, latestProgress);
}

TEST_F(BackGroundWorkerTest, Exception) {
	BackGroundWorker worker {"test", [](BackGroundWorker &, BackGroundWorker::DoTaskEventArgs &) {
		throw Framework::Exception("test", Framework::Error::Code::InvalidOperation);
	}};
	Framework::Error::Code errorCode;
	worker.TaskCompleted = [&](BackGroundWorker &, BackGroundWorker::TaskCompletedEventArgs &e) {
		errorCode = e.ErrorCode();
	};

	worker.RunTaskAsync();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(
		static_cast<int>(Framework::Error::Code::InvalidOperation),
		static_cast<int>(errorCode));
}


