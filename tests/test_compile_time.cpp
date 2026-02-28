#include <concepts>
#include <string>
#include <type_traits>

#include "../include/speed_lib.hpp"

using namespace speed_lib;

static_assert(std::same_as<std::remove_cvref_t<decltype(10_kmh)>, Speed<SPEED_UNIT::KMH, LiteralBaseInt>>);
static_assert(std::same_as<std::remove_cvref_t<decltype(10.0_kmh)>, Speed<SPEED_UNIT::KMH, LiteralBase>>);

constexpr auto signed_speed_literal = -10_kmh;
static_assert(std::same_as<decltype(signed_speed_literal), const Speed<SPEED_UNIT::KMH, LiteralBaseInt>>);
static_assert(signed_speed_literal.value == -10);

constexpr auto converted_int_speed = static_cast<Speed<SPEED_UNIT::MS, int>>(10_kmh);
static_assert(converted_int_speed.value == 2);

template <typename T>
concept SpeedPlusDoubleCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, T scalar) {
    speed + scalar;
};

static_assert(!SpeedPlusDoubleCompiles<double>);

template <typename T>
concept SpeedPlusStringCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, std::string text) {
    speed + text;
};

static_assert(!SpeedPlusStringCompiles<double>);

template <typename T>
concept SpeedDivByStringCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, std::string text) {
    speed / text;
};

static_assert(!SpeedDivByStringCompiles<double>);

template <typename T>
concept SpeedPlusTimeCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time) {
    speed + time;
};

template <typename T>
concept SpeedPlusLengthCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Length<LENGTH_UNIT::M, T> length) {
    speed + length;
};

template <typename T>
concept LengthPlusTimeCompiles = requires(Length<LENGTH_UNIT::M, T> length, Time<TIME_UNIT::S, T> time) {
    length + time;
};

static_assert(!SpeedPlusTimeCompiles<double>);
static_assert(!SpeedPlusLengthCompiles<double>);
static_assert(!LengthPlusTimeCompiles<double>);

template <typename T>
concept SpeedPlusTimePlusDoubleCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time, T scalar) {
    speed + time + scalar;
};

template <typename T>
concept SpeedPlusLengthPlusDoubleCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Length<LENGTH_UNIT::M, T> length, T scalar) {
    speed + length + scalar;
};

template <typename T>
concept LengthPlusTimePlusDoubleCompiles = requires(Length<LENGTH_UNIT::M, T> length, Time<TIME_UNIT::S, T> time, T scalar) {
    length + time + scalar;
};

static_assert(!SpeedPlusTimePlusDoubleCompiles<double>);
static_assert(!SpeedPlusLengthPlusDoubleCompiles<double>);
static_assert(!LengthPlusTimePlusDoubleCompiles<double>);

template <typename T>
concept SpeedMinusTimeCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time) {
    speed - time;
};

template <typename T>
concept SpeedMinusLengthCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Length<LENGTH_UNIT::M, T> length) {
    speed - length;
};

template <typename T>
concept LengthMinusTimeCompiles = requires(Length<LENGTH_UNIT::M, T> length, Time<TIME_UNIT::S, T> time) {
    length - time;
};

static_assert(!SpeedMinusTimeCompiles<double>);
static_assert(!SpeedMinusLengthCompiles<double>);
static_assert(!LengthMinusTimeCompiles<double>);

template <typename T>
concept SpeedMinusTimePlusDoubleCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time, T scalar) {
    speed - time + scalar;
};

template <typename T>
concept SpeedMinusLengthPlusDoubleCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Length<LENGTH_UNIT::M, T> length, T scalar) {
    speed - length + scalar;
};

template <typename T>
concept LengthMinusTimePlusDoubleCompiles = requires(Length<LENGTH_UNIT::M, T> length, Time<TIME_UNIT::S, T> time, T scalar) {
    length - time + scalar;
};

static_assert(!SpeedMinusTimePlusDoubleCompiles<double>);
static_assert(!SpeedMinusLengthPlusDoubleCompiles<double>);
static_assert(!LengthMinusTimePlusDoubleCompiles<double>);

template <typename T>
concept SpeedPlusSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> a, Speed<SPEED_UNIT::KMH, T> b) {
    a + b;
};

template <typename T>
concept TimePlusTimeCompiles = requires(Time<TIME_UNIT::S, T> a, Time<TIME_UNIT::MIN, T> b) {
    a + b;
};

template <typename T>
concept LengthPlusLengthCompiles = requires(Length<LENGTH_UNIT::M, T> a, Length<LENGTH_UNIT::KM, T> b) {
    a + b;
};

template <typename T>
concept SpeedMinusSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> a, Speed<SPEED_UNIT::KMH, T> b) {
    a - b;
};

template <typename T>
concept UnaryMinusSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> speed) {
    -speed;
};

template <typename T>
concept SpeedTimesScalarCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, T scalar) {
    speed * scalar;
};

template <typename T>
concept ScalarTimesSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, T scalar) {
    scalar * speed;
};

template <typename T>
concept SpeedDivScalarCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, T scalar) {
    speed / scalar;
};

template <typename T>
concept TimeTimesScalarCompiles = requires(Time<TIME_UNIT::S, T> time, T scalar) {
    time * scalar;
};

template <typename T>
concept ScalarTimesTimeCompiles = requires(Time<TIME_UNIT::S, T> time, T scalar) {
    scalar * time;
};

template <typename T>
concept TimeDivScalarCompiles = requires(Time<TIME_UNIT::S, T> time, T scalar) {
    time / scalar;
};

template <typename T>
concept LengthTimesScalarCompiles = requires(Length<LENGTH_UNIT::M, T> length, T scalar) {
    length * scalar;
};

template <typename T>
concept ScalarTimesLengthCompiles = requires(Length<LENGTH_UNIT::M, T> length, T scalar) {
    scalar * length;
};

template <typename T>
concept LengthDivScalarCompiles = requires(Length<LENGTH_UNIT::M, T> length, T scalar) {
    length / scalar;
};

template <typename T>
concept SpeedTimesTimeCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time) {
    speed * time;
};

template <typename T>
concept SpeedTimesSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> a, Speed<SPEED_UNIT::KMH, T> b) {
    a * b;
};

template <typename T>
concept TimeTimesTimeCompiles = requires(Time<TIME_UNIT::S, T> a, Time<TIME_UNIT::MIN, T> b) {
    a * b;
};

template <typename T>
concept LengthTimesLengthCompiles = requires(Length<LENGTH_UNIT::M, T> a, Length<LENGTH_UNIT::KM, T> b) {
    a * b;
};

template <typename T>
concept LengthDivTimeCompiles = requires(Length<LENGTH_UNIT::M, T> length, Time<TIME_UNIT::S, T> time) {
    length / time;
};

template <typename T>
concept SpeedEqualsSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> a, Speed<SPEED_UNIT::KMH, T> b) {
    { a == b } -> std::same_as<bool>;
};

template <typename T>
concept SpeedEqualsTimeCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time) {
    speed == time;
};

template <typename T>
concept SpeedSpaceshipSpeedCompiles = requires(Speed<SPEED_UNIT::MS, T> a, Speed<SPEED_UNIT::KMH, T> b) {
    a <=> b;
};

template <typename T>
concept SpeedSpaceshipTimeCompiles = requires(Speed<SPEED_UNIT::MS, T> speed, Time<TIME_UNIT::S, T> time) {
    speed <=> time;
};

static_assert(SpeedPlusSpeedCompiles<double>);
static_assert(TimePlusTimeCompiles<double>);
static_assert(LengthPlusLengthCompiles<double>);
static_assert(SpeedMinusSpeedCompiles<double>);
static_assert(UnaryMinusSpeedCompiles<double>);
static_assert(SpeedTimesScalarCompiles<double>);
static_assert(ScalarTimesSpeedCompiles<double>);
static_assert(SpeedDivScalarCompiles<double>);
static_assert(TimeTimesScalarCompiles<double>);
static_assert(ScalarTimesTimeCompiles<double>);
static_assert(TimeDivScalarCompiles<double>);
static_assert(LengthTimesScalarCompiles<double>);
static_assert(ScalarTimesLengthCompiles<double>);
static_assert(LengthDivScalarCompiles<double>);
static_assert(SpeedTimesTimeCompiles<double>);
static_assert(!SpeedTimesSpeedCompiles<double>);
static_assert(!TimeTimesTimeCompiles<double>);
static_assert(!LengthTimesLengthCompiles<double>);
static_assert(LengthDivTimeCompiles<double>);
static_assert(SpeedEqualsSpeedCompiles<double>);
static_assert(!SpeedEqualsTimeCompiles<double>);
static_assert(SpeedSpaceshipSpeedCompiles<double>);
static_assert(!SpeedSpaceshipTimeCompiles<double>);

constexpr Length<LENGTH_UNIT::M, double> one_thousand_m{1000.0};
constexpr Length<LENGTH_UNIT::KM, double> one_km_for_ratio{1.0};
static_assert((one_thousand_m / one_km_for_ratio) == 1.0);

constexpr Length<LENGTH_UNIT::KM, int> one_km_for_equality{1};
constexpr Length<LENGTH_UNIT::M, int> one_thousand_m_for_equality{1000};
static_assert(one_km_for_equality == one_thousand_m_for_equality);
static_assert(!(one_km_for_equality != one_thousand_m_for_equality));

int main()
{
    return 0;
}
