#include <array>
#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

TEST(ArithmeticTests, Addition)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, Subtraction)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_NEAR(result.value, 0.0, 1e-12);
}

TEST(ArithmeticTests, Multiplication)
{
    auto s1 = 10.0_ms;
    auto result = s1 * 2.0L;
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, MultiplicationScalarOnLeft)
{
    auto s1 = 10.0_ms;
    auto result = 2.0L * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, Division)
{
    auto s1 = 10.0_ms;
    auto result = s1 / 2.0L;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 5.0);
}

TEST(ArithmeticTests, DivisionBySpeed)
{
    auto s1 = 10.0_ms;
    auto s2 = 2.0_ms;
    auto result = s1 / s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), long double>));
    EXPECT_DOUBLE_EQ(result, 5.0);
}

TEST(ArithmeticTests, MixedUnits)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, MixedUnitsMultipleOperations)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = (s1 + s2) * 2.0L;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 40.0);
}

TEST(ArithmeticTests, IntegerAddition)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerSubtraction)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, int>>));
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, IntegerMultiplication)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 * 2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerMultiplicationScalarOnLeft)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = 2 * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerDivision)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 / 2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, int>>));
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTests, IntegerDivisionBySpeed)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{2};
    auto result = s1 / s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), int>));
    EXPECT_EQ(result, 5);
}

TEST(ArithmeticTests, UnsignedIntegerAddition)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerSubtraction)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, unsigned int>>));
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, UnsignedIntegerMultiplication)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 * 2U;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerMultiplicationScalarOnLeft)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = 2U * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerDivision)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 / 2U;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_REPRESENTATION::MS, unsigned int>>));
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTests, UnsignedIntegerDivisionBySpeed)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{2};
    auto result = s1 / s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), unsigned int>));
    EXPECT_EQ(result, 5U);
}

TEST(ArithmeticTest, ArrayAddition)
{
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> speeds1{10.0_ms, 20.0_ms, 36.0_kmh};
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> speeds2{1.0_ms, 7.2_kmh, 3.0_ms};
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> expected{39.6_kmh, 22.0_ms, 13.0_ms};

    for (size_t i = 0; i < speeds1.size(); ++i)
    {
        auto result = speeds1[i] + speeds2[i];
        EXPECT_DOUBLE_EQ(result.value, expected[i].value);
    }
}
