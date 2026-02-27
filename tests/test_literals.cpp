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