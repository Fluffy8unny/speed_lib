#include "../include/speed_lib.hpp"
#include <gtest/gtest.h>

using namespace speed_lib;

TEST(ComparisonTests, EqualitySameUnitSameValue)
{
    const Speed<SPEED_UNIT::MS, int> a{10};
    const Speed<SPEED_UNIT::MS, int> b{10};

    EXPECT_TRUE(a == b);
}

TEST(ComparisonTests, EqualitySameUnitDifferentValue)
{
    const Speed<SPEED_UNIT::MS, int> a{10};
    const Speed<SPEED_UNIT::MS, int> b{11};

    EXPECT_FALSE(a == b);
}

TEST(ComparisonTests, EqualityCrossUnitEqualValue)
{
    const Length<LENGTH_UNIT::KM, int> a{1};
    const Length<LENGTH_UNIT::M, int> b{1000};

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(b == a);
}

TEST(ComparisonTests, EqualityCrossUnitDifferentValue)
{
    const Length<LENGTH_UNIT::KM, int> a{1};
    const Length<LENGTH_UNIT::M, int> b{999};

    EXPECT_FALSE(a == b);
    EXPECT_FALSE(b == a);
}

TEST(ComparisonTests, EqualityCrossUnitMixedValueTypes)
{
    const Length<LENGTH_UNIT::KM, int> a{2};
    const Length<LENGTH_UNIT::M, long double> b{2000.0L};

    static_assert(std::is_same_v<std::remove_cvref_t<decltype(a == b)>, bool>);
    EXPECT_TRUE(a == b);
}

TEST(ComparisonTests, InequalityOperatorUsesEquality)
{
    const Time<TIME_UNIT::MIN, int> a{2};
    const Time<TIME_UNIT::S, int> b{120};

    EXPECT_FALSE(a != b);
}
