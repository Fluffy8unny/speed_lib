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

TEST(SpeedConversion, ValueTypeConversionOperator)
{
    Speed<SPEED_REPRESENTATION::KMH, double> s{36.5};
    const double value = static_cast<double>(s);
    EXPECT_DOUBLE_EQ(value, 36.5);
}

TEST(SpeedConversion, ValueTypeImplicitConversionOperator)
{
    Speed<SPEED_REPRESENTATION::KMH, double> s{36.5};
    const double value = s;
    EXPECT_DOUBLE_EQ(value, 36.5);
}

TEST(SpeedConversion, ValueTypeConversionOperatorInt)
{
    Speed<SPEED_REPRESENTATION::KMH, int> s{36};
    const int value = static_cast<int>(s);
    EXPECT_EQ(value, 36);
}

TEST(SpeedConversion, ValueTypeConversionOperatorUnsigned)
{
    Speed<SPEED_REPRESENTATION::KMH, unsigned> s{36U};
    const unsigned value = static_cast<unsigned>(s);
    EXPECT_EQ(value, 36U);
}

TEST(TimeConversion, SecondsToMinutesToHours)
{
    auto s = 3600.0_s;
    auto min = static_cast<Time<TIME_REPRESENTATION::MIN, long double>>(s);
    auto h = static_cast<Time<TIME_REPRESENTATION::H, long double>>(min);
    EXPECT_DOUBLE_EQ(min.value, 60.0);
    EXPECT_DOUBLE_EQ(h.value, 1.0);
}

TEST(TimeConversion, HoursToSeconds)
{
    Time<TIME_REPRESENTATION::H, double> h{1.5};
    auto s = static_cast<Time<TIME_REPRESENTATION::S, double>>(h);
    EXPECT_DOUBLE_EQ(s.value, 5400.0);
}

TEST(LengthConversion, MetersToKilometers)
{
    auto meters = 1500.0_m;
    auto km = static_cast<Length<LENGTH_REPRESENTATION::KM, long double>>(meters);
    EXPECT_DOUBLE_EQ(km.value, 1.5);
}

TEST(LengthConversion, MilesToFeet)
{
    auto miles = 1.0_mi;
    auto feet = static_cast<Length<LENGTH_REPRESENTATION::FT, long double>>(miles);
    EXPECT_NEAR(feet.value, 5280.0, 1e-9);
}