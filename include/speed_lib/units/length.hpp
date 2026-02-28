#pragma once

#include "../common.hpp"
#include "../detail/unit_macros.hpp"

namespace speed_lib
{
    /// @brief Units that represent length. The base unit is meter (M).
    enum class LENGTH_UNIT : char
    {
        M,  ///< Meter (base unit).
        KM, ///< Kilometer.
        MI, ///< Mile.
        FT  ///< Foot.
    };

    // This creates a type trait, that can be used to map from a dimension tag to its corresponding unit enum. For example, UnitForTag<LengthTag>::type will be LENGTH_UNIT.
    SPEED_LIB_DEFINE_TAG_WITH_UNIT(LengthTag, LENGTH_UNIT)

    // Here the actual conversion between units inside of a dimension is defined.
    // these are all defined in relation to a base unit
    // base unit is m
    SPEED_LIB_DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::M, 1.0L, "m", "m", "Length")
    SPEED_LIB_DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::KM, 1000.0L, "km", "km", "Length")
    SPEED_LIB_DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::MI, 1609.344L, "mi", "mi", "Length")
    SPEED_LIB_DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::FT, 0.3048L, "ft", "ft", "Length")

    // These are just nice aliases to avoid having to write Quantity<LengthTag, LENGTH_UNIT::M, double> all the time
    template <LENGTH_UNIT Unit, typename ValueType>
    using Length = Quantity<LengthTag, Unit, ValueType>;

    // Literals for each unit, so you can write things like auto s = 10_kmh; or auto t = 5min;
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, M, m);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, KM, km);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, MI, mi);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, FT, ft);
}

#include "../detail/unit_macros_undef.hpp"
