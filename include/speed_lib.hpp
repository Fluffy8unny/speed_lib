#pragma once

#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstring>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace speed_lib
{
    template <typename T>
    concept NumericalType = std::is_arithmetic_v<T>;

    template <typename DimensionTag>
    struct UnitForTag;
    /**
     * Define dimension tags and their corresponding unit enums.
     * Each dimension is a unit of measurement.
     */

    /// @brief Units that represent speed. The base unit is meter per second (MS).
    enum class SPEED_UNIT : char
    {
        MS,  ///< Meter per second (base unit).
        KMH, ///< Kilometer per hour.
        MPH, ///< Mile per hour.
        KNT, ///< Knot.
        C    ///< Speed of light.
    };

    /// @brief Units that represent time. The base unit is second (S).
    enum class TIME_UNIT : char
    {
        S,   ///< Second (base unit).
        MIN, ///< Minute.
        H    ///< Hour.
    };

    /// @brief Units that represent length. The base unit is meter (M).
    enum class LENGTH_UNIT : char
    {
        M,  ///< Meter (base unit).
        KM, ///< Kilometer.
        MI, ///< Mile.
        FT  ///< Foot.
    };
// This creates a type trait, that can be used to map from a dimension tag to its corresponding unit enum. For example, UnitForTag<SpeedTag>::type will be SPEED_UNIT.
#define DEFINE_TAG_WITH_UNIT(_tag, _unit) \
    struct _tag                           \
    {                                     \
    };                                    \
    template <>                           \
    struct UnitForTag<_tag>               \
    {                                     \
        using type = _unit;               \
    };

    DEFINE_TAG_WITH_UNIT(SpeedTag, SPEED_UNIT)
    DEFINE_TAG_WITH_UNIT(TimeTag, TIME_UNIT)
    DEFINE_TAG_WITH_UNIT(LengthTag, LENGTH_UNIT)

#undef DEFINE_TAG_WITH_UNIT
    // DimensionTag contains the dimension (e.g., Speed, Time, Length)
    template <typename DimensionTag>
    using UnitForTag_t = typename UnitForTag<DimensionTag>::type;

    template <typename DimensionTag, auto Unit>
    struct UnitTraits;

    // Here the actual conversion between units inside of a dimension is defined.
    // these are all defined in relation to a base unit
#define DEFINE_UNIT_TRAITS(_tag, _unit, scale, _suffix, _format_specifier, _dimension_name) \
    template <>                                                                             \
    struct UnitTraits<_tag, _unit>                                                          \
    {                                                                                       \
        static constexpr long double scale_to_base{scale};                                  \
        static constexpr const char *suffix = _suffix;                                      \
        static constexpr const char *format_specifier = _format_specifier;                  \
        static constexpr const char *dimension_name = _dimension_name;                      \
    };
    // base unit is m/s
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MS, 1.0L, "ms", "m/s", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KMH, 1000000.0L / 3600000.0L, "kmh", "km/h", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MPH, 1609344.0L / 3600000.0L, "mph", "mi/h", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KNT, 1852000.0L / 3600000.0L, "knt", "knt", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::C, 299792458.0L, "c", "c", "Speed")
    // base unit is s
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::S, 1.0L, "s", "s", "Time")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::MIN, 60.0L, "min", "min", "Time")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::H, 3600.0L, "h", "h", "Time")
    // base unit is m
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::M, 1.0L, "m", "m", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::KM, 1000.0L, "km", "km", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::MI, 1609.344L, "mi", "mi", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::FT, 0.3048L, "ft", "ft", "Length")

#undef DEFINE_UNIT_TRAITS
    // here the actual conversion between units inside of a dimension is defined
    // the factor is calculated by converting from the source unit to the base unit, then from the base unit to the target unit
    template <typename DimensionTag, UnitForTag_t<DimensionTag> UnitFrom, UnitForTag_t<DimensionTag> UnitTo, NumericalType ValueType>
    struct ConversionMap
    {
        using conversion_type = std::common_type_t<long double, ValueType>; // make sure the accuracy of the conversion is at least the max accuracy of the types involved
        static constexpr conversion_type value{UnitTraits<DimensionTag, UnitFrom>::scale_to_base / UnitTraits<DimensionTag, UnitTo>::scale_to_base};
    };

    /**
     * @brief Quantity struct represents a physical Quantity with a specific dimension (e.g., speed, time, length), unit, and underlying value type.
     * Arithmetic operations and comparisons between Quantities of the same dimension are supported, even if they are in different units of the same dimension
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam Unit Unit enum value (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric storage type (e.g. double).
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, typename ValueType>
        requires NumericalType<ValueType>
    struct Quantity
    {
        ValueType value;
        /**
         * @brief Converts the Quantity to a different unit of the same dimension.
         * @tparam TargetUnit Target unit (e.g. LENGTH_UNIT::KM).
         * @param[in] TargetUnit the unit to convert to, needs to be in the same dimension as the current unit
         * @returns A new Quantity object with the same dimension and value type, but in the target unit
         * @example auto km = 1500_m.convert<speed_lib::LENGTH_UNIT::KM>();
         */
        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr Quantity<DimensionTag, TargetUnit, ValueType> convert() const
        {
            using ConversionType = typename ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::conversion_type; // make sure the accuracy of the conversion is at least the max accuracy of the types involved.
            const auto converted_value = static_cast<ConversionType>(value) * ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::value;
            return Quantity<DimensionTag, TargetUnit, ValueType>{static_cast<ValueType>(converted_value)};
        }

        /**
         * @brief Casting from unit of the same dimension to another implicitly
         * @tparam TargetUnit Target unit (e.g. LENGTH_UNIT::KM).
         * @param[in] TargetUnit the unit to convert to needs to be in the same dimension as the current unit
         * @returns A new Quantity object with the same dimension and value type, but in the target unit
         * @example speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double> km = 1500_m;
         */
        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr operator Quantity<DimensionTag, TargetUnit, ValueType>() const
        {
            return convert<TargetUnit>();
        }

        /**
         * @brief Extracts the raw value of the Quantity.
         * @returns The underlying value of the Quantity.
         */
        constexpr operator ValueType() const
        {
            return value;
        }

        /**
         * @brief Casting to the same unit, but with another underlying value type.
         * @tparam TargetValueType Target value type (e.g. double).
         * @returns A new Quantity object with the same dimension and unit, but with the target value type.
         * @example  int x = 1_m; double y=x; y now has the value 1.0, and is of type Quantity<LengthTag, LENGTH_UNIT::M, double>
         */
        template <typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType> // god I love concepts
        constexpr operator Quantity<DimensionTag, Unit, TargetValueType>() const
        {
            return Quantity<DimensionTag, Unit, TargetValueType>{static_cast<TargetValueType>(value)};
        }

        /**
         * @brief Casting to a different unit and value type simultaneously.
         * @tparam TargetUnit Target unit (e.g. LENGTH_UNIT::KM).
         * @tparam TargetValueType Target value type (e.g. double).
         * @returns A new Quantity object with the same dimension, but in the target unit and value type.
         * @example speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double> km = static_cast<speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double>>(1500_m);
         */
        template <UnitForTag_t<DimensionTag> TargetUnit, typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType>
        constexpr operator Quantity<DimensionTag, TargetUnit, TargetValueType>() const
        {
            const auto converted_quantity = convert<TargetUnit>();
            return static_cast<Quantity<DimensionTag, TargetUnit, TargetValueType>>(converted_quantity);
        }

        // units can be compared with each other, as long as they are of the same dimension...
        constexpr auto operator<=>(const Quantity &other) const
        {
            return value <=> other.value;
        }

        // even if they are not in the same unit
        template <UnitForTag_t<DimensionTag> OtherUnit, typename OtherValueType>
            requires std::three_way_comparable_with<ValueType, OtherValueType>
        constexpr auto operator<=>(const Quantity<DimensionTag, OtherUnit, OtherValueType> &other) const
        {
            const auto converted_other = other.template convert<Unit>();
            using CommonType = std::common_type_t<ValueType, OtherValueType>;
            return static_cast<CommonType>(value) <=> static_cast<CommonType>(converted_other.value);
        }

        // someone is gonna complain if I don't do it
        friend std::ostream &operator<<(std::ostream &os, const Quantity &q)
        {
            return os << q.value << ' ' << UnitTraits<DimensionTag, Unit>::format_specifier;
        }
    };
    // These are just nice aliases to avoid having to write Quantity<SpeedTag, SPEED_UNIT::MS, double> all the time
    template <SPEED_UNIT Unit, typename ValueType>
    using Speed = Quantity<SpeedTag, Unit, ValueType>;

    template <TIME_UNIT Unit, typename ValueType>
    using Time = Quantity<TimeTag, Unit, ValueType>;

    template <LENGTH_UNIT Unit, typename ValueType>
    using Length = Quantity<LengthTag, Unit, ValueType>;

    using LiteralBase = long double;
    using LiteralBaseInt = long long;

    // Literals for each unit, so you can write things like auto s = 10_kmh; or auto t = 5min;
#define DEFINE_LITERAL_OPERATOR(DimensionTag, UnitType, unit, suffix)                                              \
    constexpr Quantity<DimensionTag, UnitType::unit, LiteralBase> operator""_##suffix(LiteralBase value)           \
    {                                                                                                              \
        return Quantity<DimensionTag, UnitType::unit, LiteralBase>{value};                                         \
    }                                                                                                              \
    constexpr Quantity<DimensionTag, UnitType::unit, LiteralBaseInt> operator""_##suffix(unsigned long long value) \
    {                                                                                                              \
        return Quantity<DimensionTag, UnitType::unit, LiteralBaseInt>{static_cast<LiteralBaseInt>(value)};         \
    }

    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, MS, ms)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, KMH, kmh)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, MPH, mph)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, KNT, knt)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_UNIT, C, c)

    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, S, s)
    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, MIN, min)
    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_UNIT, H, h)

    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, M, m)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, KM, km)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, MI, mi)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_UNIT, FT, ft)
#undef DEFINE_LITERAL_OPERATOR

///@brief We don't want invalid units. Like what is 1_mph + 5min supposed to mean?
#define DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(_operator)                                          \
    template <typename LeftDimensionTag,                                                           \
              UnitForTag_t<LeftDimensionTag> LeftUnit,                                             \
              NumericalType LeftValueType,                                                         \
              typename RightDimensionTag,                                                          \
              UnitForTag_t<RightDimensionTag> RightUnit,                                           \
              NumericalType RightValueType>                                                        \
        requires(!std::same_as<LeftDimensionTag, RightDimensionTag>)                               \
    constexpr void operator _operator(const Quantity<LeftDimensionTag, LeftUnit, LeftValueType> &, \
                                      const Quantity<RightDimensionTag, RightUnit, RightValueType> &) = delete;

///@brief We have no way at the moment to handle powers of units.
#define DEFINE_SAME_DIMENSION_OPERATOR_DELETE(_operator)                                       \
    template <typename DimensionTag,                                                           \
              UnitForTag_t<DimensionTag> LeftUnit,                                             \
              NumericalType LeftValueType,                                                     \
              UnitForTag_t<DimensionTag> RightUnit,                                            \
              NumericalType RightValueType>                                                    \
    constexpr void operator _operator(const Quantity<DimensionTag, LeftUnit, LeftValueType> &, \
                                      const Quantity<DimensionTag, RightUnit, RightValueType> &) = delete;

    // Intra dimension operations. These are defined for all combinations of units within the same dimension, and the result is in the left-hand-side unit.
    /**
     * @brief Applies a binary operation to two Quantities of the same dimension and unit.
     * @tparam Op Binary operation type (e.g. std::plus<long double>).
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit for both Quantities (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] lhs Left-hand-side Quantity.
     * @param[in] rhs Right-hand-side Quantity.
     * @param[in] op Binary operation applied to the Quantity values.
     * @return Quantity with the left unit and same value type.
     * @example auto s = speed_lib::apply_binary_op(10_m, 500_m, std::plus<long double>{});
     */
    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType>
        requires std::is_invocable_r_v<ValueType, Op, ValueType, ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> apply_binary_op(const Quantity<DimensionTag, LeftUnit, ValueType> &lhs, const Quantity<DimensionTag, LeftUnit, ValueType> &rhs, Op op)
    {
        return Quantity<DimensionTag, LeftUnit, ValueType>{op(lhs.value, rhs.value)};
    }

    /**
     * @brief Applies a binary operation after converting the RHS Quantity to the LHS unit.
     * @tparam Op Binary operation type (e.g. std::plus<long double>).
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Left unit (e.g. LENGTH_UNIT::M).
     * @tparam RightUnit Right unit (e.g. LENGTH_UNIT::KM).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] lhs Left-hand-side Quantity that determines output unit.
     * @param[in] rhs Right-hand-side Quantity, converted to the left unit before applying the operation.
     * @param[in] op Binary operation applied to the Quantity values.
     * @return Quantity with the left unit and same value type.
     * @example auto s = speed_lib::apply_binary_op(10_m, 500_m, std::plus<long double>{});
     */
    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
        requires std::is_invocable_r_v<ValueType, Op, ValueType, ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> apply_binary_op(const Quantity<DimensionTag, LeftUnit, ValueType> &lhs, const Quantity<DimensionTag, RightUnit, ValueType> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<LeftUnit>();
        return Quantity<DimensionTag, LeftUnit, ValueType>{op(lhs.value, rhs_converted.value)};
    }

    /**
     * @brief Adds two Quantities of the same dimension.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Left unit (e.g. LENGTH_UNIT::KM).
     * @tparam RightUnit Right unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] a Left-hand-side Quantity.
     * @param[in] b Right-hand-side Quantity.
     * @return Sum in the left-hand-side unit.
     * @example auto sum = 2_km + 500_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator+(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::plus<ValueType>{});
    }

    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(+)

    /**
     * @brief Subtracts two Quantities of the same dimension.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Left unit (e.g. LENGTH_UNIT::KM).
     * @tparam RightUnit Right unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] a Left-hand-side Quantity.
     * @param[in] b Right-hand-side Quantity.
     * @return Difference in the left-hand-side unit.
     * @example auto diff = 2_km - 500_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::minus<ValueType>{});
    }

    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(-)

    /**
     * @brief Unary negation for a Quantity.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] q Quantity to negate.
     * @return Quantity with negated value in the same unit.
     * @example auto neg = -5_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &q)
    {
        return Quantity<DimensionTag, LeftUnit, ValueType>{-q.value};
    }
    DEFINE_SAME_DIMENSION_OPERATOR_DELETE(*)
    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(*)

    /**
     * @brief Multiplies a Quantity by a scalar.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Quantity value type (e.g. long double).
     * @tparam Scalar Scalar type (e.g. int).
     * @param[in] s Quantity operand.
     * @param[in] a Scalar multiplier.
     * @return Quantity in the same unit with common promoted value type.
     * @example auto scaled = 10_m * 2;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator*(const Quantity<DimensionTag, LeftUnit, ValueType> &s, const Scalar a)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(s.value) * static_cast<ResultType>(a)};
    }

    /**
     * @brief Multiplies a scalar by a Quantity.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Quantity value type (e.g. long double).
     * @tparam Scalar Scalar type (e.g. int).
     * @param[in] a Scalar multiplier.
     * @param[in] s Quantity operand.
     * @return Quantity in the same unit with common promoted value type.
     * @example auto scaled = 2 * 10_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator*(const Scalar a, const Quantity<DimensionTag, LeftUnit, ValueType> &s)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(a) * static_cast<ResultType>(s.value)};
    }
    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(/)

    /**
     * @brief Divides a Quantity by a scalar.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Quantity value type (e.g. long double).
     * @tparam Scalar Scalar type (e.g. int).
     * @param[in] s Quantity operand.
     * @param[in] a Scalar divisor.
     * @return Quantity in the same unit with common promoted value type.
     * @example auto half = 10_m / 2;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator/(const Quantity<DimensionTag, LeftUnit, ValueType> &s, const Scalar &a)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(s.value) / static_cast<ResultType>(a)};
    }

#undef DEFINE_MIXED_DIMENSION_OPERATOR_DELETE
#undef DEFINE_SAME_DIMENSION_OPERATOR_DELETE
    // Inter dimension operations. These are only defined for certain combinations of dimensions, and the result is always in the base unit of the resulting dimension.

    /**
     * @brief Divides two Quantities with the same dimension and unit.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam Unit Shared unit (e.g. LENGTH_UNIT::M).
     * @tparam LeftValueType Left value type (e.g. int).
     * @tparam RightValueType Right value type (e.g. long double).
     * @param[in] a Numerator Quantity.
     * @param[in] b Denominator Quantity.
     * @return Unitless ratio.
     * @example auto ratio = 10_m / 2_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, NumericalType LeftValueType, NumericalType RightValueType>
    constexpr std::common_type_t<LeftValueType, RightValueType> operator/(const Quantity<DimensionTag, Unit, LeftValueType> &a, const Quantity<DimensionTag, Unit, RightValueType> &b)
    {
        using ResultType = std::common_type_t<LeftValueType, RightValueType>;
        return static_cast<ResultType>(a.value) / static_cast<ResultType>(b.value);
    }

    /**
     * @brief Divides two Quantities of the same dimension but possibly different units.
     * @details The RHS Quantity is converted to the LHS unit before division.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Left unit (e.g. LENGTH_UNIT::KM).
     * @tparam RightUnit Right unit (e.g. LENGTH_UNIT::M).
     * @tparam LeftValueType Left value type (e.g. int).
     * @tparam RightValueType Right value type (e.g. long double).
     * @param[in] a Numerator Quantity.
     * @param[in] b Denominator Quantity.
     * @return Unitless ratio
     * @example auto ratio = 1_km / 500_m;
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType LeftValueType, NumericalType RightValueType>
    constexpr std::common_type_t<LeftValueType, RightValueType> operator/(const Quantity<DimensionTag, LeftUnit, LeftValueType> &a, const Quantity<DimensionTag, RightUnit, RightValueType> &b)
    {
        using ResultType = std::common_type_t<LeftValueType, RightValueType>;
        const auto converted_b = b.template convert<LeftUnit>();
        return static_cast<ResultType>(a.value) / static_cast<ResultType>(converted_b.value);
    }

    /**
     * @brief Computes length from speed multiplied by time.
     * @tparam SpeedUnit Speed unit (e.g. SPEED_UNIT::MS).
     * @tparam TimeUnit Time unit (e.g. TIME_UNIT::S).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] speed Speed Quantity.
     * @param[in] time Time Quantity.
     * @return Length in meters.
     * @example auto d = 10_ms * 5_s;
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
     * @example auto v = 100_m / 5_s;
     */
    template <LENGTH_UNIT LengthUnit, TIME_UNIT TimeUnit, NumericalType ValueType>
    constexpr Speed<SPEED_UNIT::MS, ValueType> operator/(const Length<LengthUnit, ValueType> &length, const Time<TimeUnit, ValueType> &time)
    {
        const auto length_m = length.template convert<LENGTH_UNIT::M>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Speed<SPEED_UNIT::MS, ValueType>{length_m.value / time_s.value};
    }

    template <typename DimensionTag>
    struct ParsedView
    {
        const char *unit = nullptr;
        std::optional<UnitForTag_t<DimensionTag>> target_unit = std::nullopt;
        std::optional<unsigned> width = std::nullopt;
        std::optional<unsigned> precision = std::nullopt;
    };

    /**
     * @brief Parses an unsigned integer from a string view starting at a given cursor position.
     * If parsing is successful, the cursor is advanced past the parsed number.
     *
     * @param[in] s The string view to parse from.
     * @param[in,out] cursor The current position in the string view. This will be updated to point behind the number if parsing is successful.
     * @return An optional containing the parsed unsigned integer if parsing was successful, or std::nullopt
     */
    constexpr std::optional<unsigned> parse_unsigned_at(std::string_view s, std::size_t &cursor)
    {
        if (cursor >= s.size())
            return std::nullopt;

        const auto tail = s.substr(cursor);

        unsigned v{};
        const auto [ptr, ec] = std::from_chars(tail.data(), tail.data() + tail.size(), v, 10);
        if (ptr == tail.data() or ec != std::errc{})
            return std::nullopt;

        cursor += static_cast<std::size_t>(ptr - tail.data());
        return v;
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> ParsedUnit>
    constexpr bool try_parse_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        const auto suffix = UnitTraits<DimensionTag, ParsedUnit>::suffix;
        if (!s.substr(cursor).starts_with(suffix))
            return false;

        out.unit = UnitTraits<DimensionTag, ParsedUnit>::format_specifier;
        out.target_unit = ParsedUnit;
        cursor += std::strlen(suffix);
        return true;
    }

    // check if any of the provided units for this dimension can be parsed
    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr bool try_parse_any_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        return (try_parse_unit<DimensionTag, Units>(s, cursor, out) or ...);
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr std::optional<ParsedView<DimensionTag>> parse_quantity(std::string_view s)
    {
        // We're parsing in 3 steps, that represent the 3 main components of a format specifier: the unit, the width, and the precision
        // The unit is mandatory, while the width and precision are optional
        std::size_t cursor = 0;
        auto consume_character = [&](char c) constexpr -> bool
        {
            return cursor < s.size() and s[cursor] == c ? (++cursor, true) : false;
        };
        // parse the unit
        ParsedView<DimensionTag> result{};
        if (!try_parse_any_unit<DimensionTag, Units...>(s, cursor, result))
            return std::nullopt; // no unit means it's not a valid format specifier

        // try to parse the width, which is an optional unsigned integer that comes after the unit
        if (auto w = parse_unsigned_at(s, cursor))
            result.width = *w;
        // try to parse the precision, which is an optional unsigned integer that comes after a '.' character, and is followed by an 'f'
        // this is meant to mirror the syntax of the precision specifier for floating point types in the standard library
        if (consume_character('.'))
        {
            if (auto p = parse_unsigned_at(s, cursor); p)
                result.precision = *p;
            else
                return std::nullopt;
            // can't forget the f
            if (!consume_character('f'))
                return std::nullopt;
        }
        // there is junk at the end, this is invalid
        if (cursor != s.size())
            return std::nullopt;

        return result;
    }

    // I'm willing to admit that using a variadic template here to generate an error message was just done for style points and is not strictly necessary
    /**
     * @brief Builds an error message for invalid format specifiers, listing the valid units for the given dimension.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam FirstUnit First valid unit (e.g. SPEED_UNIT::MS).
     * @tparam RemainingUnits Remaining valid units (e.g. SPEED_UNIT::KMH).
     * @example auto msg = speed_lib::build_formatter_error_message<speed_lib::SpeedTag, speed_lib::SPEED_UNIT::MS>();
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> FirstUnit, UnitForTag_t<DimensionTag>... RemainingUnits>
    std::string build_formatter_error_message()
    {
        std::string units;
        auto append_quoted_suffix = [&](const char *suffix)
        {
            units += std::format("{}'{}'", units.empty() ? "" : ", ", suffix);
        };

        append_quoted_suffix(UnitTraits<DimensionTag, FirstUnit>::suffix);
        (append_quoted_suffix(UnitTraits<DimensionTag, RemainingUnits>::suffix), ...);

        return std::format(
            "invalid format specifier for {}. Use {}, optionally followed by a number with an 'f' suffix (e.g., '{}8.2f').",
            UnitTraits<DimensionTag, FirstUnit>::dimension_name,
            units,
            UnitTraits<DimensionTag, FirstUnit>::suffix);
    }
    /**
     * @brief Formatter for Quantities. Prints the value followed by the unit.
     * Also parses the format string (:UNITWIDTH.PRECISIONf e.g. km10.4f) to allow for optional unit conversion and formatting options.
     * @note For example, "{:kmh8.2f}" would print the Quantity in km/h with a width of 8 and a precision of 2.
     * @tparam DimensionTag Dimension category (e.g. SpeedTag).
     * @tparam Unit Source unit (e.g. SPEED_UNIT::MS).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @tparam ValidUnits Allowed target units (e.g. SPEED_UNIT::KMH).
     * @example std::format("{:kmh8.2f}", 10_ms);
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, NumericalType ValueType, UnitForTag_t<DimensionTag>... ValidUnits>
    struct QuantityFormatter : std::formatter<ValueType, char>
    {
        ParsedView<DimensionTag> parsed_format;
        /**
         * @brief Converts the Quantity to the target unit if the target unit matches the candidate unit e.g. m->km
         * @tparam CandidateUnit Current candidate unit (e.g. SPEED_UNIT::KMH).
         * @tparam RemainingUnits Remaining candidate units (e.g. SPEED_UNIT::MPH).
         * @param[in] target_unit Requested target unit.
         * @param[in] quantity Source Quantity.
         * @param[out] formatted_value Converted numeric value in the matched candidate unit.
         * @example // Used internally by QuantityFormatter during format conversion.
         */
        template <UnitForTag_t<DimensionTag> CandidateUnit, UnitForTag_t<DimensionTag>... RemainingUnits>
        static constexpr void convert_if_unit_matches(
            UnitForTag_t<DimensionTag> target_unit,
            const Quantity<DimensionTag, Unit, ValueType> &quantity,
            ValueType &formatted_value)
        {
            if (target_unit == CandidateUnit)
            {
                formatted_value = quantity.template convert<CandidateUnit>().value;
                return;
            }

            if constexpr (sizeof...(RemainingUnits) > 0)
                convert_if_unit_matches<RemainingUnits...>(target_unit, quantity, formatted_value);
        }
        /**
         * @brief Parses the format specifier for the Quantity.
         * @param[in,out] ctx Format parse context consumed by the formatter.
         */
        constexpr auto parse(std::format_parse_context &ctx)
        {
            auto it = ctx.begin();
            auto end = ctx.end();

            if (it == end or *it == '}')
            {
                parsed_format = ParsedView<DimensionTag>{};
                return it;
            }

            auto start = it;
            it = std::find(it, end, '}');

            std::string_view spec{&*start, static_cast<size_t>(it - start)};
            if (auto parsed = parse_quantity<DimensionTag, ValidUnits...>(spec); parsed.has_value())
                parsed_format = *parsed;
            else
                throw std::format_error(build_formatter_error_message<DimensionTag, ValidUnits...>());

            return it;
        }
        /**
         * @brief Formats the Quantity according to the parsed format specifier.
         * @tparam FormatContext Formatting context type (e.g. std::format_context).
         * @param[in] quantity Quantity to format.
         * @param[in,out] ctx Output format context receiving formatted text.
         * @example std::format("{}", 10_ms);
         */
        template <typename FormatContext>
        auto format(const Quantity<DimensionTag, Unit, ValueType> &quantity, FormatContext &ctx) const
        {
            const auto unit = parsed_format.unit != nullptr
                                  ? parsed_format.unit
                                  : UnitTraits<DimensionTag, Unit>::format_specifier;

            ValueType formatted_value = quantity.value;
            if (parsed_format.target_unit.has_value())
            {
                const auto target_unit = *parsed_format.target_unit;
                convert_if_unit_matches<ValidUnits...>(target_unit, quantity, formatted_value);
            }

            if (parsed_format.width and parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:>{}.{}f}", static_cast<long double>(formatted_value), *parsed_format.width, *parsed_format.precision);
            }
            else if (parsed_format.width)
            {
                std::format_to(ctx.out(), "{:>{}}", formatted_value, *parsed_format.width);
            }
            else if (parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:.{}f}", static_cast<long double>(formatted_value), *parsed_format.precision);
            }
            else
            {
                std::format_to(ctx.out(), "{}", formatted_value);
            }

            return std::format_to(ctx.out(), " {}", unit);
        }
    };
}

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