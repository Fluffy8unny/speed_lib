#pragma once

#include "../common.hpp"
#include "../formatting.hpp"
#include "../detail/unit_macros.hpp"

namespace speed_lib
{
    /// @brief Units that represent temperature. The base unit is kelvin (K).
    enum class TEMPERATURE_UNIT : char
    {
        K, ///< Kelvin (base unit).
        C, ///< Celsius.
        F  ///< Fahrenheit.
    };

    // This creates a type trait, that can be used to map from a dimension tag to its corresponding unit enum.
    SPEED_LIB_DEFINE_TAG_WITH_UNIT(TemperatureTag, TEMPERATURE_UNIT)

    // Here the actual conversion between units inside of a dimension is defined.
    // these are all defined in relation to a base unit
    // base unit is K
    SPEED_LIB_DEFINE_UNIT_TRAITS(TemperatureTag, TEMPERATURE_UNIT::K, 1.0L, 0.0L, "k", "K", "Temperature")
    SPEED_LIB_DEFINE_UNIT_TRAITS(TemperatureTag, TEMPERATURE_UNIT::C, 1.0L, 273.15L, "degc", "°C", "Temperature")
    SPEED_LIB_DEFINE_UNIT_TRAITS(TemperatureTag, TEMPERATURE_UNIT::F, 5.0L / 9.0L, 273.15L - (32.0L * 5.0L / 9.0L), "degf", "°F", "Temperature")

    // These are just nice aliases to avoid having to write Quantity<TemperatureTag, TEMPERATURE_UNIT::K, double> all the time
    template <TEMPERATURE_UNIT Unit, typename ValueType>
    using Temperature = Quantity<TemperatureTag, Unit, ValueType>;

    // Literals for each unit, so you can write things like auto t = 100_degc;
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TemperatureTag, TEMPERATURE_UNIT, K, k);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TemperatureTag, TEMPERATURE_UNIT, C, degc);
    SPEED_LIB_DEFINE_LITERAL_OPERATOR(TemperatureTag, TEMPERATURE_UNIT, F, degf);
}

/**
 * @brief Formatter specialization for Temperature.
 * @tparam Unit Temperature unit (e.g. TEMPERATURE_UNIT::C).
 * @tparam ValueType Numeric value type (e.g. long double).
 * @code
 * std::format("{}", 100_degc);
 * @endcode
 */
template <speed_lib::TEMPERATURE_UNIT Unit, speed_lib::NumericalType ValueType>
struct std::formatter<speed_lib::Temperature<Unit, ValueType>>
    : speed_lib::QuantityFormatter<
          speed_lib::TemperatureTag,
          Unit,
          ValueType,
          speed_lib::TEMPERATURE_UNIT::K,
          speed_lib::TEMPERATURE_UNIT::C,
          speed_lib::TEMPERATURE_UNIT::F>
{
};

#include "../detail/unit_macros_undef.hpp"
