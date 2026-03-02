#pragma once

#include <compare>
#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>

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

    template <typename DimensionTag>
    using UnitForTag_t = typename UnitForTag<DimensionTag>::type;

    template <typename DimensionTag, auto Unit>
    struct UnitTraits;

    using LiteralBase = long double;
    using LiteralBaseInt = long long;

    template <typename DimensionTag, UnitForTag_t<DimensionTag> UnitFrom, UnitForTag_t<DimensionTag> UnitTo, NumericalType ValueType>
    struct ConversionMap
    {
        // make sure the accuracy of the conversion is at least the max accuracy of the types involved
        using conversion_type = std::common_type_t<LiteralBase, ValueType>;
        static constexpr conversion_type scale{UnitTraits<DimensionTag, UnitFrom>::scale_to_base / UnitTraits<DimensionTag, UnitTo>::scale_to_base};
        // Affine conversion with base-space relation b = x * s + o:
        // y = (x * s_from + o_from - o_to) / s_to
        //   = x * (s_from / s_to) + (o_from - o_to) / s_to
        static constexpr conversion_type offset{
            (UnitTraits<DimensionTag, UnitFrom>::offset_to_base - UnitTraits<DimensionTag, UnitTo>::offset_to_base) / UnitTraits<DimensionTag, UnitTo>::scale_to_base};
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
         * @details TargetUnit must be in the same dimension as the current unit.
         * @returns A new Quantity object with the same dimension and value type, but in the target unit
         * @code
         * auto km = 1500_m.convert<speed_lib::LENGTH_UNIT::KM>();
         * @endcode
         */
        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr Quantity<DimensionTag, TargetUnit, ValueType> convert() const
        {
            // make sure the accuracy of the conversion is at least the max accuracy of the types involved.
            using ConversionType = typename ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::conversion_type;
            const auto converted_value = static_cast<ConversionType>(value) * ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::scale +
                                         ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::offset;
            return Quantity<DimensionTag, TargetUnit, ValueType>{static_cast<ValueType>(converted_value)};
        }

        /**
         * @brief Casting from unit of the same dimension to another implicitly
         * @tparam TargetUnit Target unit (e.g. LENGTH_UNIT::KM).
         * @details TargetUnit must be in the same dimension as the current unit.
         * @returns A new Quantity object with the same dimension and value type, but in the target unit
         * @code
         * speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double> km = 1500_m;
         * @endcode
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
        constexpr explicit operator ValueType() const
        {
            return value;
        }

        /**
         * @brief Casting to the same unit, but with another underlying value type.
         * @tparam TargetValueType Target value type (e.g. double).
         * @returns A new Quantity object with the same dimension and unit, but with the target value type.
         * @code
         * int x = 1_m; double y = x; // y now has the value 1.0, and is of type Quantity<LengthTag, LENGTH_UNIT::M, double>
         * @endcode
         */
        template <typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType>
        constexpr operator Quantity<DimensionTag, Unit, TargetValueType>() const
        {
            return Quantity<DimensionTag, Unit, TargetValueType>{static_cast<TargetValueType>(value)};
        }

        /**
         * @brief Casting to a different unit and value type simultaneously.
         * @tparam TargetUnit Target unit (e.g. LENGTH_UNIT::KM).
         * @tparam TargetValueType Target value type (e.g. double).
         * @returns A new Quantity object with the same dimension, but in the target unit and value type.
         * @code
         * speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double> km = static_cast<speed_lib::Length<speed_lib::LENGTH_UNIT::KM, double>>(1500_m);
         * @endcode
         */
        template <UnitForTag_t<DimensionTag> TargetUnit, typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType>
        constexpr operator Quantity<DimensionTag, TargetUnit, TargetValueType>() const
        {
            const auto converted_quantity = convert<TargetUnit>();
            return static_cast<Quantity<DimensionTag, TargetUnit, TargetValueType>>(converted_quantity);
        }

        /**
         * @brief Three-way comparison for Quantities with identical dimension, unit, and value type.
         * @param[in] other Quantity to compare against.
         * @return Comparison category result of the underlying values.
         * @code
         * auto cmp = 10_m <=> 12_m;
         * @endcode
         */
        constexpr auto operator<=>(const Quantity &other) const
        {
            return value <=> other.value;
        }

        /**
         * @brief Equality comparison for Quantities with identical dimension, unit, and value type.
         * @param[in] other Quantity to compare against.
         * @return true if underlying values are equal, otherwise false.
         * @code
         * bool eq = 10_m == 10_m;
         * @endcode
         */
        constexpr bool operator==(const Quantity &other) const
        {
            return value == other.value;
        }

        /**
         * @brief Normalizes this Quantity and another same-dimension Quantity to a common base-space value type.
         * @tparam OtherUnit Unit of the right-hand-side Quantity.
         * @tparam OtherValueType Value type of the right-hand-side Quantity.
         * @param[in] other Quantity to normalize alongside this Quantity.
         * @return Pair of values converted to base units and promoted to a common type.
         */
        template <UnitForTag_t<DimensionTag> OtherUnit, typename OtherValueType>
        constexpr auto to_common_base_pair(const Quantity<DimensionTag, OtherUnit, OtherValueType> &other) const
        {
            using LeftBaseType = typename ConversionMap<DimensionTag, Unit, Unit, ValueType>::conversion_type;
            using RightBaseType = typename ConversionMap<DimensionTag, OtherUnit, OtherUnit, OtherValueType>::conversion_type;

            const auto lhs_in_base = static_cast<LeftBaseType>(value) * UnitTraits<DimensionTag, Unit>::scale_to_base +
                                     UnitTraits<DimensionTag, Unit>::offset_to_base;
            const auto rhs_in_base = static_cast<RightBaseType>(other.value) * UnitTraits<DimensionTag, OtherUnit>::scale_to_base +
                                     UnitTraits<DimensionTag, OtherUnit>::offset_to_base;

            using CommonType = std::common_type_t<LeftBaseType, RightBaseType>;
            return std::pair<CommonType, CommonType>{
                static_cast<CommonType>(lhs_in_base),
                static_cast<CommonType>(rhs_in_base)};
        }

        /**
         * @brief Three-way comparison for Quantities of the same dimension but different units and/or value types.
         * @details The RHS Quantity is converted to the LHS unit before comparison.
         * @tparam OtherUnit Unit of the right-hand-side Quantity.
         * @tparam OtherValueType Value type of the right-hand-side Quantity.
         * @param[in] other Quantity to compare against.
         * @return Comparison category result after unit conversion and common-type promotion.
         * @code
         * auto cmp = 1_km <=> 900_m;
         * @endcode
         */
        template <UnitForTag_t<DimensionTag> OtherUnit, typename OtherValueType>
            requires std::three_way_comparable_with<ValueType, OtherValueType>
        constexpr auto operator<=>(const Quantity<DimensionTag, OtherUnit, OtherValueType> &other) const
        {
            const auto [lhs_in_base, rhs_in_base] = to_common_base_pair(other);
            return lhs_in_base <=> rhs_in_base;
        }

        /**
         * @brief Equality comparison for Quantities of the same dimension but different units and/or value types.
         * @details The RHS Quantity is converted to the LHS unit before comparison.
         * @tparam OtherUnit Unit of the right-hand-side Quantity.
         * @tparam OtherValueType Value type of the right-hand-side Quantity.
         * @param[in] other Quantity to compare against.
         * @return true if values are equal after unit conversion and common-type promotion, otherwise false.
         * @code
         * bool eq = 1_km == 1000_m;
         * @endcode
         */
        template <UnitForTag_t<DimensionTag> OtherUnit, typename OtherValueType>
            requires std::equality_comparable_with<ValueType, OtherValueType>
        constexpr bool operator==(const Quantity<DimensionTag, OtherUnit, OtherValueType> &other) const
        {
            const auto [lhs_in_base, rhs_in_base] = to_common_base_pair(other);
            return lhs_in_base == rhs_in_base;
        }

        /**
         * @brief Streams a Quantity to an output stream in the format "<value> <unit suffix>".
         * @param[in,out] os Output stream.
         * @param[in] q Quantity to stream.
         * @return Reference to the output stream.
         * @code
         * std::cout << 42_m;
         * @endcode
         */
        friend std::ostream &operator<<(std::ostream &os, const Quantity &q)
        {
            return os << q.value << ' ' << UnitTraits<DimensionTag, Unit>::format_specifier;
        }
    };

///@brief We don't want invalid units. Like what is 1_mph + 5min supposed to mean?
#define SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(_operator)                                \
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
#define SPEED_LIB_DEFINE_SAME_DIMENSION_OPERATOR_DELETE(_operator)                             \
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
     * @code
     * auto s = speed_lib::apply_binary_op(10_m, 500_m, std::plus<long double>{});
     * @endcode
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
     * @code
     * auto s = speed_lib::apply_binary_op(10_m, 500_m, std::plus<long double>{});
     * @endcode
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
     * @code
     * auto sum = 2_km + 500_m;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator+(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::plus<ValueType>{});
    }

    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(+)
    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(==)
    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(<=>)

    /**
     * @brief Subtracts two Quantities of the same dimension.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Left unit (e.g. LENGTH_UNIT::KM).
     * @tparam RightUnit Right unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] a Left-hand-side Quantity.
     * @param[in] b Right-hand-side Quantity.
     * @return Difference in the left-hand-side unit.
     * @code
     * auto diff = 2_km - 500_m;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::minus<ValueType>{});
    }

    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(-)

    /**
     * @brief Unary negation for a Quantity.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Numeric value type (e.g. long double).
     * @param[in] q Quantity to negate.
     * @return Quantity with negated value in the same unit.
     * @code
     * auto neg = -5_m;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &q)
    {
        return Quantity<DimensionTag, LeftUnit, ValueType>{-q.value};
    }

    SPEED_LIB_DEFINE_SAME_DIMENSION_OPERATOR_DELETE(*)
    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(*)

    /**
     * @brief Multiplies a Quantity by a scalar.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Quantity value type (e.g. long double).
     * @tparam Scalar Scalar type (e.g. int).
     * @param[in] s Quantity operand.
     * @param[in] a Scalar multiplier.
     * @return Quantity in the same unit with common promoted value type.
     * @code
     * auto scaled = 10_m * 2;
     * @endcode
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
     * @code
     * auto scaled = 2 * 10_m;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator*(const Scalar a, const Quantity<DimensionTag, LeftUnit, ValueType> &s)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(a) * static_cast<ResultType>(s.value)};
    }

    SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(/)

    /**
     * @brief Divides a Quantity by a scalar.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam LeftUnit Unit (e.g. LENGTH_UNIT::M).
     * @tparam ValueType Quantity value type (e.g. long double).
     * @tparam Scalar Scalar type (e.g. int).
     * @param[in] s Quantity operand.
     * @param[in] a Scalar divisor.
     * @return Quantity in the same unit with common promoted value type.
     * @code
     * auto half = 10_m / 2;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator/(const Quantity<DimensionTag, LeftUnit, ValueType> &s, const Scalar &a)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(s.value) / static_cast<ResultType>(a)};
    }

    /**
     * @brief Divides two Quantities with the same dimension and unit.
     * @tparam DimensionTag Dimension category (e.g. LengthTag).
     * @tparam Unit Shared unit (e.g. LENGTH_UNIT::M).
     * @tparam LeftValueType Left value type (e.g. int).
     * @tparam RightValueType Right value type (e.g. long double).
     * @param[in] a Numerator Quantity.
     * @param[in] b Denominator Quantity.
     * @return Unitless ratio.
     * @code
     * auto ratio = 10_m / 2_m;
     * @endcode
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
     * @code
     * auto ratio = 1_km / 500_m;
     * @endcode
     */
    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType LeftValueType, NumericalType RightValueType>
    constexpr std::common_type_t<LeftValueType, RightValueType> operator/(const Quantity<DimensionTag, LeftUnit, LeftValueType> &a, const Quantity<DimensionTag, RightUnit, RightValueType> &b)
    {
        using ResultType = std::common_type_t<LeftValueType, RightValueType>;
        const auto converted_b = b.template convert<LeftUnit>();
        return static_cast<ResultType>(a.value) / static_cast<ResultType>(converted_b.value);
    }

#undef SPEED_LIB_DEFINE_MIXED_DIMENSION_OPERATOR_DELETE
#undef SPEED_LIB_DEFINE_SAME_DIMENSION_OPERATOR_DELETE
}
