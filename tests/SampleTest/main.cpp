#include "gtest/gtest.h"
#include "hello.h"

TEST(SampleTest, sample) {
	testing::internal::CaptureStdout();
	Hello();
	EXPECT_STREQ("Hello Cpp Learning project!!\n", testing::internal::GetCapturedStdout().c_str());
}
