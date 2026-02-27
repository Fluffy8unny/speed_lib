#include <array>
#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

TEST(ArithmeticTests, Addition)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, Subtraction)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
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
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, MultiplicationScalarOnLeftWithDifferentScalarTypeKeepsQuantity)
{
    auto s1 = 10.0_ms;
    auto result = 2.0 * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0L);
}

TEST(ArithmeticTests, Division)
{
    auto s1 = 10.0_ms;
    auto result = s1 / 2.0L;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
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

TEST(ArithmeticTests, DivisionBySpeedSameUnitMixedValueTypes)
{
    const Speed<SPEED_UNIT::MS, int> s1{10};
    const Speed<SPEED_UNIT::MS, double> s2{2.0};
    const auto result = s1 / s2;

    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(result)>, double>));
    EXPECT_DOUBLE_EQ(result, 5.0);
}

TEST(ArithmeticTests, DivisionBySpeedDifferentUnitsMixedValueTypes)
{
    const Speed<SPEED_UNIT::MS, int> s1{10};
    const Speed<SPEED_UNIT::KMH, double> s2{36.0};
    const auto result = s1 / s2;

    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(result)>, double>));
    EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(ArithmeticTests, DivisionByLengthImplicitUnitConversion)
{
    const Length<LENGTH_UNIT::M, double> meters{1000.0};
    const Length<LENGTH_UNIT::KM, double> kilometers{1.0};
    const auto result = meters / kilometers;

    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(result)>, double>));
    EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(ArithmeticTests, MixedUnits)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, MixedUnitsMultipleOperations)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = (s1 + s2) * 2.0L;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 40.0);
}

TEST(ArithmeticTests, IntegerAddition)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, int>{10};
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerSubtraction)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, int>{10};
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, int>>));
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, IntegerMultiplication)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto result = s1 * 2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerMultiplicationScalarOnLeft)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto result = 2 * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerMultiplicationScalarOnLeftImplicitCast)
{
    const auto speed = Speed<SPEED_UNIT::MS, int>{10};
    const auto intermediate = 0.5 * speed;
    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(intermediate)>, Speed<SPEED_UNIT::MS, double>>));

    const Speed<SPEED_UNIT::MS, int> result = intermediate;
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTests, IntegerDivision)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto result = s1 / 2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, int>>));
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTests, IntegerDivisionBySpeed)
{
    auto s1 = Speed<SPEED_UNIT::MS, int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, int>{2};
    auto result = s1 / s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), int>));
    EXPECT_EQ(result, 5);
}

TEST(ArithmeticTests, UnsignedIntegerAddition)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto result = s1 + s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerSubtraction)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto result = s1 - s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, unsigned int>>));
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, UnsignedIntegerMultiplication)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto result = s1 * 2U;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerMultiplicationScalarOnLeft)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto result = 2U * s1;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, unsigned int>>));
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerDivision)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto result = s1 / 2U;
    EXPECT_TRUE((std::is_same_v<decltype(result), Speed<SPEED_UNIT::MS, unsigned int>>));
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTests, UnsignedIntegerDivisionBySpeed)
{
    auto s1 = Speed<SPEED_UNIT::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_UNIT::MS, unsigned int>{2};
    auto result = s1 / s2;
    EXPECT_TRUE((std::is_same_v<decltype(result), unsigned int>));
    EXPECT_EQ(result, 5U);
}

TEST(ArithmeticTest, ArrayAddition)
{
    std::array<Speed<SPEED_UNIT::MS, double>, 3> speeds1{10.0_ms, 20.0_ms, 36.0_kmh};
    std::array<Speed<SPEED_UNIT::MS, double>, 3> speeds2{1.0_ms, 7.2_kmh, 3.0_ms};
    std::array<Speed<SPEED_UNIT::MS, double>, 3> expected{39.6_kmh, 22.0_ms, 13.0_ms};

    for (size_t i = 0; i < speeds1.size(); ++i)
    {
        auto result = speeds1[i] + speeds2[i];
        EXPECT_DOUBLE_EQ(result.value, expected[i].value);
    }
}

TEST(ArithmeticTests, SpeedTimeMultiplicationProducesLength)
{
    const Speed<SPEED_UNIT::MS, long double> speed{10.0L};
    const Time<TIME_UNIT::S, long double> time{5.0L};

    const Length<LENGTH_UNIT::M, long double> result = speed * time;

    static_assert(std::is_same_v<decltype(speed * time), Length<LENGTH_UNIT::M, long double>>);
    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(result)>, Length<LENGTH_UNIT::M, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 50.0L);
}

TEST(ArithmeticTests, LengthTimeDivisionProducesSpeed)
{
    const Length<LENGTH_UNIT::M, long double> length{120.0L};
    const Time<TIME_UNIT::S, long double> time{10.0L};

    const Speed<SPEED_UNIT::MS, long double> result = length / time;

    static_assert(std::is_same_v<decltype(length / time), Speed<SPEED_UNIT::MS, long double>>);
    EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(result)>, Speed<SPEED_UNIT::MS, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 12.0L);
}

TEST(ArithmeticTests, LengthTimeDivisionAndSpeedTimeMultiplicationRoundTrip)
{
    const Length<LENGTH_UNIT::KM, long double> original_length{1.2L};
    const Time<TIME_UNIT::MIN, long double> time{2.0L};

    const Speed<SPEED_UNIT::MS, long double> speed = original_length / time;
    const Length<LENGTH_UNIT::M, long double> reconstructed_length = speed * time;

    static_assert(std::is_same_v<decltype(original_length / time), Speed<SPEED_UNIT::MS, long double>>);
    static_assert(std::is_same_v<decltype(speed * time), Length<LENGTH_UNIT::M, long double>>);

    EXPECT_DOUBLE_EQ(speed.value, 10.0L);
    EXPECT_DOUBLE_EQ(reconstructed_length.value, 1200.0L);
}

TEST(ArithmeticTests, SpeedTimeMultiplicationMixedUnits)
{
    const Speed<SPEED_UNIT::KMH, long double> speed{36.0L};
    const Time<TIME_UNIT::H, long double> time{1.0L};

    const Length<LENGTH_UNIT::M, long double> result = speed * time;

    static_assert(std::is_same_v<decltype(speed * time), Length<LENGTH_UNIT::M, long double>>);
    EXPECT_DOUBLE_EQ(result.value, 36000.0L);
}

TEST(ArithmeticTests, SpaceshipOperatorWithConvertibleQuantities)
{
    const Length<LENGTH_UNIT::KM, long double> one_km{1.0L};
    const Length<LENGTH_UNIT::M, long double> five_hundred_m{500.0L};
    EXPECT_TRUE((one_km <=> five_hundred_m) > 0);

    const Length<LENGTH_UNIT::M, int> one_thousand_m_int{1000};
    const Length<LENGTH_UNIT::KM, double> one_km_double{1.0};
    EXPECT_TRUE((one_thousand_m_int <=> one_km_double) == 0);

    const Time<TIME_UNIT::H, long double> two_hours{2.0L};
    const Time<TIME_UNIT::MIN, long double> ninety_minutes{90.0L};
    EXPECT_TRUE((two_hours <=> ninety_minutes) > 0);

    const Time<TIME_UNIT::S, int> three_thousand_six_hundred_seconds{3600};
    const Time<TIME_UNIT::H, double> one_hour{1.0};
    EXPECT_TRUE((three_thousand_six_hundred_seconds <=> one_hour) == 0);

    const Speed<SPEED_UNIT::KMH, long double> seventy_two_kmh{72.0L};
    const Speed<SPEED_UNIT::MS, long double> ten_ms{10.0L};
    EXPECT_TRUE((seventy_two_kmh <=> ten_ms) > 0);

    const Speed<SPEED_UNIT::MS, int> ten_ms_int{10};
    const Speed<SPEED_UNIT::KMH, double> thirty_six_kmh{36.0};
    EXPECT_TRUE((ten_ms_int <=> thirty_six_kmh) == 0);
}

TEST(TimeArithmetic, MixedUnitsAddition)
{
    auto result = 30.0_s + 1.0_min;
    EXPECT_TRUE((std::is_same_v<decltype(result), Time<TIME_UNIT::S, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 90.0);
}

TEST(LengthArithmetic, MixedUnitsSubtraction)
{
    auto result = 1.0_km - 100.0_m;
    EXPECT_TRUE((std::is_same_v<decltype(result), Length<LENGTH_UNIT::KM, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 0.9);
}
