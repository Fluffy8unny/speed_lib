#pragma once

#include "../common.hpp"
#include "../formatting.hpp"
#include "../detail/unit_macros.hpp"

namespace speed_lib
{
    /// @brief Units that represent speed. The base unit is meter per second (MS).
    enum class SPEED_UNIT : char
    {
        MS,  ///< Meter per second (base unit).
        KMH, ///< Kilometer per hour.
        MPH, ///< Mile per hour.
        KNT, ///< Knot.
        C    ///< Speed of light.
    };

    // This creates a type trait, that can be used to map from a dimension tag to its corresponding unit enum. For example, UnitForTag<SpeedTag>::type will be SPEED_UNIT.
    SPEED_LIB_DEFINE_TAG_WITH_UNIT(SpeedTag, SPEED_UNIT)

    // Here the actual conversion between units inside of a dimension is defined.
    // these are all defined in relation to a base unit
    // base unit is m/s
    SPEED_LIB_DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MS, 1.0L, 0.0L, "ms", "m/s", "Speed")
    SPEED_LIB_DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KMH, 1000000.0L / 3600000.0L, 0.0L, "kmh", "km/h", "Speed")
    SPEED_LIB_DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MPH, 1609344.0L / 3600000.0L, 0.0L, "mph", "mi/h", "Speed")
    SPEED_LIB_DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KNT, 1852000.0L / 3600000.0L, 0.0L, "knt", "knt", "Speed")
    SPEED_LIB_DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::C, 299792458.0L, 0.0L, "c", "c", "Speed")

    // These are just nice aliases to avoid having to write Quantity<SpeedTag, SPEED_UNIT::MS, double> all the time
    template <SPEED_UNIT Unit, typename ValueType>
    using Speed = Quantity<SpeedTag, Unit, ValueType>;

    // Literals for each unit, so you can write things like auto s = 10_kmh; or auto t = 5min;
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, MS, ms);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, KMH, kmh);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, MPH, mph);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, KNT, knt);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, C, c);
}

/**
 * @brief Formatter specialization for Speed.
 * @tparam Unit Speed unit (e.g. SPEED_UNIT::KMH).
 * @tparam ValueType Numeric value type (e.g. long double).
 * @code
 * std::format("{}", 10_kmh);
 * @endcode
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

#include "../detail/unit_macros_undef.hpp"
