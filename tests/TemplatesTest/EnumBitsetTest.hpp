#pragma once

#include <iostream>
#include "gtest/gtest.h"
#include "Templates/EnumBitset.hpp"

using namespace Framework::Templates;

enum class BitsetTestEnum {
	Value1 = 1 << 0,
	Value2 = 1 << 1,
	Value3 = 1 << 2,
	Value4 = 1 << 3,
	Value5 = 1 << 4
};

class EnumBitsetTest : public ::testing::Test {
protected:
	EnumBitset<BitsetTestEnum> bitset;
};

TEST_F(EnumBitsetTest, DefaultConstructor) {
	EXPECT_TRUE(bitset.None());
}

TEST_F(EnumBitsetTest, InitializerListConstructor) {
	EnumBitset<BitsetTestEnum> bitset({ BitsetTestEnum::Value1, BitsetTestEnum::Value3 });

	EXPECT_TRUE(bitset.Test(BitsetTestEnum::Value1));
	EXPECT_TRUE(bitset.Test(BitsetTestEnum::Value3));
	EXPECT_FALSE(bitset.Test(BitsetTestEnum::Value2));
}

TEST_F(EnumBitsetTest, SingleValueConstructor) {
	EnumBitset<BitsetTestEnum> bitset(BitsetTestEnum::Value2);
	EXPECT_TRUE(bitset.Test(BitsetTestEnum::Value2));
	EXPECT_FALSE(bitset.Test(BitsetTestEnum::Value1));
}

TEST_F(EnumBitsetTest, AndOperator) {
	EnumBitset<BitsetTestEnum> bitset1({ BitsetTestEnum::Value1, BitsetTestEnum::Value2 });
	EnumBitset<BitsetTestEnum> bitset2({ BitsetTestEnum::Value2, BitsetTestEnum::Value3 });
	bitset1 &= bitset2;
	EXPECT_FALSE(bitset1.Test(BitsetTestEnum::Value1));
	EXPECT_TRUE(bitset1.Test(BitsetTestEnum::Value2));
	EXPECT_FALSE(bitset1.Test(BitsetTestEnum::Value3));
}

TEST_F(EnumBitsetTest, OrOperator) {
	EnumBitset<BitsetTestEnum> bitset1({ BitsetTestEnum::Value1 });
	EnumBitset<BitsetTestEnum> bitset2({ BitsetTestEnum::Value2 });
	bitset1 |= bitset2;
	EXPECT_TRUE(bitset1.Test(BitsetTestEnum::Value1));
	EXPECT_TRUE(bitset1.Test(BitsetTestEnum::Value2));
}

TEST_F(EnumBitsetTest, XorOperator) {
	EnumBitset<BitsetTestEnum> bitset1({ BitsetTestEnum::Value1, BitsetTestEnum::Value2 });
	EnumBitset<BitsetTestEnum> bitset2({ BitsetTestEnum::Value2, BitsetTestEnum::Value3 });
	bitset1 ^= bitset2;
	EXPECT_TRUE(bitset1.Test(BitsetTestEnum::Value1));
	EXPECT_FALSE(bitset1.Test(BitsetTestEnum::Value2));
	EXPECT_TRUE(bitset1.Test(BitsetTestEnum::Value3));
}

TEST_F(EnumBitsetTest, SetAll) {
	bitset.Set();
	EXPECT_TRUE(bitset.All());
}

TEST_F(EnumBitsetTest, SetSingle) {
	bitset.Set(BitsetTestEnum::Value1);
	EXPECT_TRUE(bitset.Test(BitsetTestEnum::Value1));
	EXPECT_FALSE(bitset.Test(BitsetTestEnum::Value2));
}

TEST_F(EnumBitsetTest, ResetAll) {
	bitset.Set();
	bitset.Reset();
	EXPECT_TRUE(bitset.None());
}

TEST_F(EnumBitsetTest, ResetSingle) {
	bitset.Set(BitsetTestEnum::Value1);
	bitset.Reset(BitsetTestEnum::Value1);
	EXPECT_FALSE(bitset.Test(BitsetTestEnum::Value1));
}

TEST_F(EnumBitsetTest, FlipAll) {
	bitset.Flip();
	EXPECT_TRUE(bitset.All());
}

TEST_F(EnumBitsetTest, FlipSingle) {
	bitset.Flip(BitsetTestEnum::Value1);
	EXPECT_TRUE(bitset.Test(BitsetTestEnum::Value1));
	bitset.Flip(BitsetTestEnum::Value1);
	EXPECT_FALSE(bitset.Test(BitsetTestEnum::Value1));
}

TEST_F(EnumBitsetTest, Count) {
	bitset.Set(BitsetTestEnum::Value1);
	bitset.Set(BitsetTestEnum::Value2);
	EXPECT_EQ(2, bitset.Count());
}

TEST_F(EnumBitsetTest, Any) {
	bitset.Set(BitsetTestEnum::Value1);
	EXPECT_TRUE(bitset.Any());
}

TEST_F(EnumBitsetTest, None) {
	EXPECT_TRUE(bitset.None());
}

TEST_F(EnumBitsetTest, EqualityOperator) {
	EnumBitset<BitsetTestEnum> bitset1({ BitsetTestEnum::Value1 });
	EnumBitset<BitsetTestEnum> bitset2({ BitsetTestEnum::Value1 });
	EXPECT_TRUE(bitset1 == bitset2);
}

TEST_F(EnumBitsetTest, InequalityOperator) {
	EnumBitset<BitsetTestEnum> bitset1({ BitsetTestEnum::Value1 });
	EnumBitset<BitsetTestEnum> bitset2({ BitsetTestEnum::Value2 });
	EXPECT_TRUE(bitset1 != bitset2);
}

TEST_F(EnumBitsetTest, ToUnderlying) {
	EnumBitset<BitsetTestEnum> bitset({ BitsetTestEnum::Value1, BitsetTestEnum::Value3 });
	EXPECT_EQ(static_cast<typename EnumBitset<BitsetTestEnum>::UnderlyingType>(BitsetTestEnum::Value1) |
		static_cast<typename EnumBitset<BitsetTestEnum>::UnderlyingType>(BitsetTestEnum::Value3),
		bitset.ToUnderlying());
}
