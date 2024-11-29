#pragma once

#include "gtest/gtest.h"
#include "Templates/EnumOperations.hpp"
#include "Templates/EnumCast.hpp"

using namespace Framework::Templates::EnumOperations;

enum class TestEnum {
	None = 0,
	Flag1 = 1 << 0,
	Flag2 = 1 << 1,
	Flag3 = 1 << 2,
	All = Flag1 | Flag2 | Flag3
};

namespace Framework::Templates::EnumOperations::Concept {
	template <>
	struct HasBitwiseOperators<TestEnum> : std::true_type {};
}

class EnumOperationsTest : public ::testing::Test {
public:
	using Cast = Framework::Templates::EnumCast<TestEnum>;
};

TEST_F(EnumOperationsTest, BitwiseAnd) {
	TestEnum result = TestEnum::Flag1 & TestEnum::Flag2;
	EXPECT_EQ(0, Cast::ToUnderlying(result));

	result = TestEnum::Flag1 & TestEnum::Flag1;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1), Cast::ToUnderlying(result));
}

TEST_F(EnumOperationsTest, BitwiseAndAssignment) {
	TestEnum value = TestEnum::Flag1;
	value &= TestEnum::Flag2;
	EXPECT_EQ(0, Cast::ToUnderlying(value));

	value = TestEnum::Flag1;
	value &= TestEnum::Flag1;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1), Cast::ToUnderlying(value));
}

TEST_F(EnumOperationsTest, BitwiseOr) {
	TestEnum result = TestEnum::Flag1 | TestEnum::Flag2;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1) | Cast::ToUnderlying(TestEnum::Flag2), Cast::ToUnderlying(result));

	result = TestEnum::Flag1 | TestEnum::Flag1;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1), Cast::ToUnderlying(result));
}

TEST_F(EnumOperationsTest, BitwiseOrAssignment) {
	TestEnum value = TestEnum::Flag1;
	value |= TestEnum::Flag2;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1) | Cast::ToUnderlying(TestEnum::Flag2), Cast::ToUnderlying(value));

	value = TestEnum::Flag1;
	value |= TestEnum::Flag1;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1), Cast::ToUnderlying(value));
}

TEST_F(EnumOperationsTest, BitwiseNot) {
	TestEnum result = ~TestEnum::Flag1;
	EXPECT_EQ(~Cast::ToUnderlying(TestEnum::Flag1), Cast::ToUnderlying(result));
}

TEST_F(EnumOperationsTest, BitwiseXor) {
	TestEnum result = TestEnum::Flag1 ^ TestEnum::Flag2;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1) ^ Cast::ToUnderlying(TestEnum::Flag2), Cast::ToUnderlying(result));

	result = TestEnum::Flag1 ^ TestEnum::Flag1;
	EXPECT_EQ(0, Cast::ToUnderlying(result));
}

TEST_F(EnumOperationsTest, BitwiseXorAssignment) {
	TestEnum value = TestEnum::Flag1;
	value ^= TestEnum::Flag2;
	EXPECT_EQ(Cast::ToUnderlying(TestEnum::Flag1) ^ Cast::ToUnderlying(TestEnum::Flag2), Cast::ToUnderlying(value));

	value = TestEnum::Flag1;
	value ^= TestEnum::Flag1;
	EXPECT_EQ(0, Cast::ToUnderlying(value));
}





