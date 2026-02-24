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