#pragma once

#include "../common.hpp"
#include "../formatting.hpp"
#include "../detail/unit_macros.hpp"

namespace speed_lib
{
    /// @brief Units that represent time. The base unit is second (S).
    enum class TIME_UNIT : char
    {
        S,   ///< Second (base unit).
        MIN, ///< Minute.
        H    ///< Hour.
    };

    // This creates a type trait, that can be used to map from a dimension tag to its corresponding unit enum. For example, UnitForTag<TimeTag>::type will be TIME_UNIT.
    SPEED_LIB_DEFINE_TAG_WITH_UNIT(TimeTag, TIME_UNIT)

    // Here the actual conversion between units inside of a dimension is defined.
    // these are all defined in relation to a base unit
    // base unit is s
    SPEED_LIB_DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::S, 1.0L, "s", "s", "Time")
    SPEED_LIB_DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::MIN, 60.0L, "min", "min", "Time")
    SPEED_LIB_DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::H, 3600.0L, "h", "h", "Time")

    // These are just nice aliases to avoid having to write Quantity<TimeTag, TIME_UNIT::S, double> all the time
    template <TIME_UNIT Unit, typename ValueType>
    using Time = Quantity<TimeTag, Unit, ValueType>;

    // Literals for each unit, so you can write things like auto s = 10_kmh; or auto t = 5min;
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, S, s);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, MIN, min);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, H, h);
}

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

#include "../detail/unit_macros_undef.hpp"
