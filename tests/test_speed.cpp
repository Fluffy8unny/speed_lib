#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

TEST(SpeedConversion, MpsToKmh)
{
    Speed<SPEED_REPRESENTATION::MS, double> s{10.0};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}

TEST(SpeedConversion, KmhToMps)
{
    Speed<SPEED_REPRESENTATION::KMH, double> s{36.0};
    auto mps = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_DOUBLE_EQ(mps.value, 10.0);
}

TEST(SpeedConversion, RoundTrip)
{
    Speed<SPEED_REPRESENTATION::MS, double> s{12.345};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    auto back = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(kmh);
    EXPECT_NEAR(back.value, s.value, 1e-12);
}

TEST(LiteralTests, MpsLiteral)
{
    auto s = 10.0_ms;
    EXPECT_DOUBLE_EQ(s.value, 10.0);
}

TEST(LiteralTests, KmhLiteral)
{
    auto s = 36.0_kmh;
    EXPECT_DOUBLE_EQ(s.value, 36.0);
}

TEST(ArithmeticTests, Addition)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 + s2;
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, Subtraction)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 - s2;
    EXPECT_NEAR(result.value, 0.0, 1e-12);
}

TEST(ArithmeticTests, Multiplication)
{
    auto s1 = 10.0_ms;
    auto s2 = 2.0_ms;
    auto result = s1 * s2;
    EXPECT_DOUBLE_EQ(result.value, 20.0);
}

TEST(ArithmeticTests, Division)
{
    auto s1 = 10.0_ms;
    auto s2 = 2.0_ms;
    auto result = s1 / s2;
    EXPECT_DOUBLE_EQ(result.value, 5.0);
}

TEST(ArithmeticTests, Negation)
{
    auto s = 10.0_ms;
    auto result = -s;
    EXPECT_DOUBLE_EQ(result.value, -10.0);
}

TEST(ArithmeticTests, MixedUnits)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto result = s1 * s2;
    EXPECT_DOUBLE_EQ(result.value, 100.0);
}

TEST(ArithmeticTests, MixedUnitsMultipleOperations)
{
    auto s1 = 10.0_ms;
    auto s2 = 36.0_kmh;
    auto s3 = 2.0_ms;
    auto result = (s1 + s2) * s3;
    EXPECT_DOUBLE_EQ(result.value, 40.0);
}

TEST(ArithmeticTests, IntegerAddition)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 + s2;
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerSubtraction)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = s1 - s2;
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, IntegerMultiplication)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{2};
    auto result = s1 * s2;
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, IntegerDivision)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, int>{2};
    auto result = s1 / s2;
    EXPECT_EQ(result.value, 5);
}

TEST(FormatterTests, DefaultFormat)
{
    auto s = 10.0_ms;
    std::string formatted = std::format("{}", s);
    EXPECT_EQ(formatted, "10 m/s");
}

TEST(FormatterTests, MpsFormat)
{
    auto s = 10.0_ms;
    std::string formatted = std::format("{:ms}", s);
    EXPECT_EQ(formatted, "10 m/s");
}

TEST(FormatterTests, KmhFormat)
{
    auto s = 36.0_kmh;
    std::string formatted = std::format("{:kmh}", s);
    EXPECT_EQ(formatted, "36 km/h");
}

TEST(LiteralCastTests, MpsToKmh)
{
    auto s = 10.0_ms;
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}

TEST(LiteralCastTests, KmhToMps)
{
    auto s = 36.0_kmh;
    auto mps = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_DOUBLE_EQ(mps.value, 10.0);
}

TEST(LiteralCastTests, MpsToMps)
{
    auto s = 10.0_ms;
    auto mps = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_DOUBLE_EQ(mps.value, 10.0);
}

TEST(LiteralCastTests, KmhToKmh)
{
    auto s = 36.0_kmh;
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}