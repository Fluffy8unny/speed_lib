#pragma once

#include "formatting.hpp"

// Formatter specializations live in this header.
// Add custom std::formatter specializations for additional types here.

/**
 * @brief Formatter specialization for Speed.
 * @tparam Unit Speed unit (e.g. SPEED_UNIT::KMH).
 * @tparam ValueType Numeric value type (e.g. long double).
 * @example std::format("{}", 10_kmh);
 */
template <speed_lib::SPEED_UNIT Unit, speed_lib::NumericalType ValueType>
struct std::formatter<speed_lib::Speed<Unit, ValueType>>
    : speed_lib::QuantityFormatter<
          speed_lib::SpeedTag,
          Unit,
          ValueType,
          speed_lib::SPEED_UNIT::MS,
          speed_lib::SPEED_UNIT::KMH,
          speed_lib::SPEED_UNIT::MPH,
          speed_lib::SPEED_UNIT::KNT,
          speed_lib::SPEED_UNIT::C>
{
};

/**
 * @brief Formatter specialization for Time.
 * @tparam Unit Time unit (e.g. TIME_UNIT::S).
 * @tparam ValueType Numeric value type (e.g. long double).
 * @example std::format("{}", 5_s);
 */
template <speed_lib::TIME_UNIT Unit, speed_lib::NumericalType ValueType>
struct std::formatter<speed_lib::Time<Unit, ValueType>>
    : speed_lib::QuantityFormatter<
          speed_lib::TimeTag,
          Unit,
          ValueType,
          speed_lib::TIME_UNIT::S,
          speed_lib::TIME_UNIT::MIN,
          speed_lib::TIME_UNIT::H>
{
};

/**
 * @brief Formatter specialization for Length.
 * @tparam Unit Length unit (e.g. LENGTH_UNIT::M).
 * @tparam ValueType Numeric value type (e.g. long double).
 * @example std::format("{}", 42_m);
 */
template <speed_lib::LENGTH_UNIT Unit, speed_lib::NumericalType ValueType>
struct std::formatter<speed_lib::Length<Unit, ValueType>>
    : speed_lib::QuantityFormatter<
          speed_lib::LengthTag,
          Unit,
          ValueType,
          speed_lib::LENGTH_UNIT::KM,
          speed_lib::LENGTH_UNIT::MI,
          speed_lib::LENGTH_UNIT::FT,
          speed_lib::LENGTH_UNIT::M>
{
};
