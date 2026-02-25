#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

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

TEST(TimeArithmetic, MixedUnitsAddition)
{
    auto result = 30.0_s + 1.0_min;
    EXPECT_TRUE((std::is_same_v<decltype(result), Time<TIME_REPRESENTATION::S, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 90.0);
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

TEST(LengthArithmetic, MixedUnitsSubtraction)
{
    auto result = 1.0_km - 100.0_m;
    EXPECT_TRUE((std::is_same_v<decltype(result), Length<LENGTH_REPRESENTATION::KM, long double>>));
    EXPECT_DOUBLE_EQ(result.value, 0.9);
}

TEST(TimeLengthFormatting, BasicFormatting)
{
    auto t = 90.0_s;
    auto l = 1.5_km;

    EXPECT_EQ(std::format("{:s}", t), "90 s");
    EXPECT_EQ(std::format("{:km}", l), "1.5 km");
}
