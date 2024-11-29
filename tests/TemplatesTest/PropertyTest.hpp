#include <stdexcept>
#include "gtest/gtest.h"
#include "Templates/Property.hpp"

class PropertyTest : public ::testing::Test {};

namespace PropertyUnitTest {
	using namespace Framework::Templates;

	class Getter {
	public:
		static int Get(const int &value) noexcept {
			return value * 2;
		}
	};

	struct PropertyHours {
		struct Setter {
			static void Set(double &value, const double &newValue) {
				if (newValue < 0 || newValue > 24)
					throw std::out_of_range("The valid range is between 0 and 24.");
				value = newValue * 3600;
			}
		};
		struct Getter {
			static double Get(const double &value) noexcept {
				return value / 3600;
			}
		};
	};

	class FunctionSetterTest {
		std::function<int(int)> _function;
	public:
		ReferenceProperty::FunctionSetter<int(int)> Function{ _function };
		int Do(int value) {
			return _function(value);
		}
	};
}

using namespace Framework::Templates;

TEST_F(PropertyTest, ReferenceProperty_ReadOnly_IntVariable) {
	int value = 42;
	ReferenceProperty::ReadOnly<int> property(value);
	const int &actual = property;
	// property = 0; // Error: cannot assign to a read-only property
	EXPECT_EQ(42, actual);
}

TEST_F(PropertyTest, ReferenceProperty_ReadOnly_ConstVariable) {
	const int value = 42;
	ReferenceProperty::ReadOnly<int> property(value);
	int actual = property;
	EXPECT_EQ(42, actual);
}

TEST_F(PropertyTest, ReferenceProperty_ReadOnly_CustomGetter) {
	int value = 42;
	constexpr int expected = 42 * 2;
	ReferenceProperty::ReadOnly<int, PropertyUnitTest::Getter> property(value);
	int actual = property;
	EXPECT_EQ(expected, actual);
}

TEST_F(PropertyTest, ReferenceProperty_ReadOnly_OutputStream) {
	int value = 42;
	ReferenceProperty::ReadOnly<int> property(value);
	std::cout << property << std::endl;
}

TEST_F(PropertyTest, ReferenceProperty_Writable_IntVariable) {
	int value = 42;
	ReferenceProperty::Writable<int> property(value);

	int actual = property;
	EXPECT_EQ(42, actual);
	property = 0;
	int actual2 = property;
	EXPECT_EQ(0, actual2);
}

TEST_F(PropertyTest, ReferenceProperty_Writable_CustomGetterAndSetter) {
	double seconds = 0;
	ReferenceProperty::Writable<double,
		PropertyUnitTest::PropertyHours::Getter,
		PropertyUnitTest::PropertyHours::Setter> property(seconds);

	property = 12;

	double actual = property;

	EXPECT_EQ(12, actual);
	EXPECT_EQ(12 * 3600, seconds);
}

TEST_F(PropertyTest, FunctionSetter) {
	PropertyUnitTest::FunctionSetterTest test;
	test.Function = [](int value) { return value * 2; };
	int actual = test.Do(42);
	EXPECT_EQ(42 * 2, actual);
}

TEST_F(PropertyTest, Property_Writable) {
	int value = 0;
	Property<int>::Writable writableProperty{
		[&]() -> int { return value; },
		[&](const int &newValue) { value = newValue; }
	};

	writableProperty = 10;
	int actual = writableProperty;
	EXPECT_EQ(10, actual);
}

TEST_F(PropertyTest, Property_ReadOnly) {
	int value = 10;
	Property<int>::ReadOnly readOnlyProperty{
		[&]() -> int { return value; }
	};

	// readOnlyProperty = 20; // Error: cannot assign to a read-only property
	int actual = readOnlyProperty;
	EXPECT_EQ(10, actual);
}
