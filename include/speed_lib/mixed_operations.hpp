#pragma once

#include "units/length.hpp"
#include "units/speed.hpp"
#include "units/time.hpp"

namespace speed_lib
{
    // Inter dimension operations. These are only defined for certain combinations of dimensions, and the result is always in the base unit of the resulting dimension.

    /**
     * @brief Computes length from speed multiplied by time.
     * @tparam SpeedUnit Speed unit (e.g. SPEED_UNIT::MS).
     * @tparam TimeUnit Time unit (e.g. TIME_UNIT::S).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] speed Speed Quantity.
     * @param[in] time Time Quantity.
     * @return Length in meters.
     * @code
     * auto d = 10_ms * 5_s;
     * @endcode
     */
    template <SPEED_UNIT SpeedUnit, TIME_UNIT TimeUnit, NumericalType ValueType>
    constexpr Length<LENGTH_UNIT::M, ValueType> operator*(const Speed<SpeedUnit, ValueType> &speed, const Time<TimeUnit, ValueType> &time)
    {
        const auto speed_ms = speed.template convert<SPEED_UNIT::MS>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Length<LENGTH_UNIT::M, ValueType>{speed_ms.value * time_s.value};
    }

    /**
     * @brief Computes speed from length divided by time.
     * @tparam LengthUnit Length unit (e.g. LENGTH_UNIT::M).
     * @tparam TimeUnit Time unit (e.g. TIME_UNIT::S).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] length Length Quantity.
     * @param[in] time Time Quantity.
     * @return Speed in meters per second.
     * @code
     * auto v = 100_m / 5_s;
     * @endcode
     */
    template <LENGTH_UNIT LengthUnit, TIME_UNIT TimeUnit, NumericalType ValueType>
    constexpr Speed<SPEED_UNIT::MS, ValueType> operator/(const Length<LengthUnit, ValueType> &length, const Time<TimeUnit, ValueType> &time)
    {
        const auto length_m = length.template convert<LENGTH_UNIT::M>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Speed<SPEED_UNIT::MS, ValueType>{length_m.value / time_s.value};
    }
}
