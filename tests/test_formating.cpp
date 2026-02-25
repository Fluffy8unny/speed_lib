#include <array>
#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

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


TEST(TimeFormatting, AllUnits)
{
    auto t_s = 90.0_s;
    auto t_min = 1.5_min;
    auto t_h = 2.0_h;

    EXPECT_EQ(std::format("{:s}", t_s), "90 s");
    EXPECT_EQ(std::format("{:min}", t_min), "1.5 min");
    EXPECT_EQ(std::format("{:h}", t_h), "2 h");
}

TEST(LengthFormatting, AllUnits)
{
    auto l_m = 90.0_m;
    auto l_km = 1.5_km;
    auto l_mi = 2.0_mi;
    auto l_ft = 3.0_ft;

    EXPECT_EQ(std::format("{:m}", l_m), "90 m");
    EXPECT_EQ(std::format("{:km}", l_km), "1.5 km");
    EXPECT_EQ(std::format("{:mi}", l_mi), "2 mi");
    EXPECT_EQ(std::format("{:ft}", l_ft), "3 ft");
}
