#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

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

TEST(Literals, IntegerSpeedLiteralTypeAndValue)
{
    constexpr auto s = 10_kmh;
    static_assert(std::is_same_v<decltype(s), const Speed<SPEED_UNIT::KMH, LiteralBaseInt>>);
    EXPECT_EQ(s.value, 10LL);

    const auto s_ms = static_cast<Speed<SPEED_UNIT::MS, int>>(s);
    EXPECT_EQ(s_ms.value, 2);
}

TEST(Literals, NegativeIntegerSpeedLiteral)
{
    constexpr auto s = -10_kmh;
    static_assert(std::is_same_v<decltype(s), const Speed<SPEED_UNIT::KMH, LiteralBaseInt>>);
    EXPECT_EQ(s.value, -10LL);

    const auto s_ms = static_cast<Speed<SPEED_UNIT::MS, int>>(s);
    EXPECT_EQ(s_ms.value, -2);
}

TEST(LiteralCastTests, MpsToKmh)
{
    auto s = 10.0_ms;
    auto kmh = static_cast<Speed<SPEED_UNIT::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}

TEST(LiteralCastTests, KmhToMps)
{
    auto s = 36.0_kmh;
    auto mps = static_cast<Speed<SPEED_UNIT::MS, double>>(s);
    EXPECT_DOUBLE_EQ(mps.value, 10.0);
}

TEST(LiteralCastTests, MpsToMps)
{
    auto s = 10.0_ms;
    auto mps = static_cast<Speed<SPEED_UNIT::MS, double>>(s);
    EXPECT_DOUBLE_EQ(mps.value, 10.0);
}

TEST(LiteralCastTests, KmhToKmh)
{
    auto s = 36.0_kmh;
    auto kmh = static_cast<Speed<SPEED_UNIT::KMH, double>>(s);
    EXPECT_DOUBLE_EQ(kmh.value, 36.0);
}

TEST(LiteralCastTests, KntToMps)
{
    auto s = 10.0_knt;
    auto mps = static_cast<Speed<SPEED_UNIT::MS, double>>(s);
    EXPECT_NEAR(mps.value, 5.14444, 1e-4);
}

TEST(LiteralCastTests, CToMps)
{
    auto s = 1.0_c;
    auto mps = static_cast<Speed<SPEED_UNIT::MS, double>>(s);
    EXPECT_NEAR(mps.value, 299792458.0, 1e-4);
}

TEST(LiteralTests, KelvinLiteral)
{
    auto t = 273.15_k;
    EXPECT_DOUBLE_EQ(t.value, 273.15);
}

TEST(LiteralTests, CelsiusLiteral)
{
    auto t = 100.0_degc;
    EXPECT_DOUBLE_EQ(t.value, 100.0);
}

TEST(LiteralTests, FahrenheitLiteral)
{
    auto t = 32.0_degf;
    EXPECT_DOUBLE_EQ(t.value, 32.0);
}

TEST(Literals, IntegerTemperatureLiteralTypeAndValue)
{
    constexpr auto t = 273_k;
    static_assert(std::is_same_v<decltype(t), const Temperature<TEMPERATURE_UNIT::K, LiteralBaseInt>>);
    EXPECT_EQ(t.value, 273LL);

    const auto t_c = static_cast<Temperature<TEMPERATURE_UNIT::C, int>>(t);
    EXPECT_EQ(t_c.value, 0);
}

TEST(Literals, NegativeTemperatureLiteralCelsiusEqualsFahrenheitAtMinus40)
{
    constexpr auto t_c = -40_degc;
    static_assert(std::is_same_v<decltype(t_c), const Temperature<TEMPERATURE_UNIT::C, LiteralBaseInt>>);
    EXPECT_EQ(t_c.value, -40LL);

    const auto t_f = static_cast<Temperature<TEMPERATURE_UNIT::F, int>>(t_c);
    EXPECT_EQ(t_f.value, -40);
}