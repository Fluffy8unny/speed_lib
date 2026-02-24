#include <array>
#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

TEST(SpeedConversion, MsToKmh)
{
    Speed<SPEED_REPRESENTATION::MS, double> s{10.0};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}

TEST(SpeedConversion, MsToMph)
{
    Speed<SPEED_REPRESENTATION::MS, double> s{10.0};
    auto mph = static_cast<Speed<SPEED_REPRESENTATION::MPH, double>>(s);
    EXPECT_NEAR(mph.value, 22.3694, 1e-4);
}

TEST(SpeedConversion, KmhToMs)
{
    Speed<SPEED_REPRESENTATION::KMH, double> s{36.0};
    auto ms = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_DOUBLE_EQ(ms.value, 10.0);
}

TEST(SpeedConversion, KmhToMph)
{
    Speed<SPEED_REPRESENTATION::KMH, double> s{36.0};
    auto mph = static_cast<Speed<SPEED_REPRESENTATION::MPH, double>>(s);
    EXPECT_NEAR(mph.value, 22.3694, 1e-4);
}

TEST(SpeedConversion, MphToMs)
{
    Speed<SPEED_REPRESENTATION::MPH, double> s{22.3694};
    auto ms = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_NEAR(ms.value, 10.0, 1e-4);
}

TEST(SpeedConversion, MphToKmh)
{
    Speed<SPEED_REPRESENTATION::MPH, double> s{22.3694};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_NEAR(kmh.value, 36.0, 1e-4);
}

TEST(SpeedConversion, KntToMs)
{
    Speed<SPEED_REPRESENTATION::KNT, double> s{10.0};
    auto ms = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_NEAR(ms.value, 5.144444444444445, 1e-12);
}

TEST(SpeedConversion, KntToKmh)
{
    Speed<SPEED_REPRESENTATION::KNT, double> s{10.0};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_NEAR(kmh.value, 18.52, 1e-4);
}

TEST(SpeedConversion, KntToMph)
{
    Speed<SPEED_REPRESENTATION::KNT, double> s{10.0};
    auto mph = static_cast<Speed<SPEED_REPRESENTATION::MPH, double>>(s);
    EXPECT_NEAR(mph.value, 11.507794480235427, 1e-9);
}

TEST(SpeedConversion, KntToC)
{
    Speed<SPEED_REPRESENTATION::KNT, double> s{582749918.3585314};
    auto c = static_cast<Speed<SPEED_REPRESENTATION::C, double>>(s);
    EXPECT_NEAR(c.value, 1.0, 1e-9);
}

TEST(SpeedConversion, CToMs)
{
    Speed<SPEED_REPRESENTATION::C, double> s{1.0};
    auto ms = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_NEAR(ms.value, 299792458.0, 1e-4);
}

TEST(SpeedConversion, CToKmh)
{
    Speed<SPEED_REPRESENTATION::C, double> s{1.0};
    auto kmh = static_cast<Speed<SPEED_REPRESENTATION::KMH, double>>(s);
    EXPECT_NEAR(kmh.value, 1079252848.8, 1e-4);
}

TEST(SpeedConversion, CToMph)
{
    Speed<SPEED_REPRESENTATION::C, double> s{1.0};
    auto mph = static_cast<Speed<SPEED_REPRESENTATION::MPH, double>>(s);
    EXPECT_NEAR(mph.value, 670616629.3843951, 1e-4);
}

TEST(SpeedConversion, CToKnt)
{
    Speed<SPEED_REPRESENTATION::C, double> s{1.0};
    auto knt = static_cast<Speed<SPEED_REPRESENTATION::KNT, double>>(s);
    EXPECT_NEAR(knt.value, 582749918.3585314, 1e-4);
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

TEST(LiteralTests, KntLiteral)
{
    auto s = 20.0_knt;
    EXPECT_DOUBLE_EQ(s.value, 20.0);
}

TEST(LiteralTests, CLiteral)
{
    auto s = 1.0_c;
    EXPECT_DOUBLE_EQ(s.value, 1.0);
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

TEST(ArithmeticTests, IntegerNegation)
{
    auto s = Speed<SPEED_REPRESENTATION::MS, int>{10};
    auto result = -s;
    EXPECT_EQ(result.value, -10);
}

TEST(ArithmeticTests, UnsignedIntegerAddition)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 + s2;
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerSubtraction)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto result = s1 - s2;
    EXPECT_EQ(result.value, 0);
}

TEST(ArithmeticTests, UnsignedIntegerMultiplication)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{2};
    auto result = s1 * s2;
    EXPECT_EQ(result.value, 20);
}

TEST(ArithmeticTests, UnsignedIntegerDivision)
{
    auto s1 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{10};
    auto s2 = Speed<SPEED_REPRESENTATION::MS, unsigned int>{2};
    auto result = s1 / s2;
    EXPECT_EQ(result.value, 5);
}

TEST(ArithmeticTest, ArrayAddition)
{
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> speeds1{10.0_ms, 20.0_ms, 36.0_kmh};
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> speeds2{1.0_ms, 2.0_ms, 3.0_ms};
    std::array<Speed<SPEED_REPRESENTATION::MS, double>, 3> expected{11.0_ms, 22.0_ms, 13.0_ms};

    for (size_t i = 0; i < speeds1.size(); ++i)
    {
        auto result = speeds1[i] + speeds2[i];
        EXPECT_DOUBLE_EQ(result.value, expected[i].value);
    }
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

TEST(FormatterTests, KntFormat)
{
    auto s = 20.0_knt;
    std::string formatted = std::format("{:knt}", s);
    EXPECT_EQ(formatted, "20 knt");
}

TEST(FormatterTests, CFormat)
{
    auto s = 1.0_c;
    std::string formatted = std::format("{:c}", s);
    EXPECT_EQ(formatted, "1 c");
}

TEST(FormatterTests, Width)
{
    auto s = 36.0_kmh;
    std::string formatted = std::format("{:kmh5}", s);
    EXPECT_EQ(formatted, "   36 km/h");
}

TEST(FormatterTests, Precision)
{
    auto s = 36.0_kmh;
    std::string formatted = std::format("{:kmh.5f}", s);
    EXPECT_EQ(formatted, "36.00000 km/h");
}

TEST(FormatterTests, PrecisionAndWidth)
{
    auto s = 36.0_kmh;
    std::string formatted = std::format("{:kmh10.2f}", s);
    EXPECT_EQ(formatted, "     36.00 km/h");
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

TEST(LiteralCastTests, KntToMps)
{
    auto s = 10.0_knt;
    auto mps = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_NEAR(mps.value, 5.14444, 1e-4);
}

TEST(LiteralCastTests, CToMps)
{
    auto s = 1.0_c;
    auto mps = static_cast<Speed<SPEED_REPRESENTATION::MS, double>>(s);
    EXPECT_NEAR(mps.value, 299792458.0, 1e-4);
}