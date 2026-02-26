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

    enum class SPEED_UNIT : char
    {
        MS,
        KMH,
        MPH,
        KNT,
        C
    };

    enum class TIME_UNIT : char
    {
        S,
        MIN,
        H
    };

    enum class LENGTH_UNIT : char
    {
        M,
        KM,
        MI,
        FT
    };

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

    template <typename DimensionTag>
    using UnitForTag_t = typename UnitForTag<DimensionTag>::type;

    template <typename DimensionTag, auto Unit>
    struct UnitTraits;

#define DEFINE_UNIT_TRAITS(_tag, _unit, scale, _suffix, _format_specifier, _dimension_name) \
    template <>                                                                             \
    struct UnitTraits<_tag, _unit>                                                          \
    {                                                                                       \
        static constexpr long double scale_to_base{scale};                                  \
        static constexpr const char *suffix = _suffix;                                      \
        static constexpr const char *format_specifier = _format_specifier;                  \
        static constexpr const char *dimension_name = _dimension_name;                      \
    };

    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MS, 1.0L, "ms", "m/s", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KMH, 1000000.0L / 3600000.0L, "kmh", "km/h", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MPH, 1609344.0L / 3600000.0L, "mph", "mi/h", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KNT, 1852000.0L / 3600000.0L, "knt", "knt", "Speed")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::C, 299792458.0L, "c", "c", "Speed")

    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::S, 1.0L, "s", "s", "Time")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::MIN, 60.0L, "min", "min", "Time")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::H, 3600.0L, "h", "h", "Time")

    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::M, 1.0L, "m", "m", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::KM, 1000.0L, "km", "km", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::MI, 1609.344L, "mi", "mi", "Length")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::FT, 0.3048L, "ft", "ft", "Length")

#undef DEFINE_UNIT_TRAITS

    template <typename DimensionTag, UnitForTag_t<DimensionTag> UnitFrom, UnitForTag_t<DimensionTag> UnitTo, NumericalType ValueType>
    struct ConversionMap
    {
        using conversion_type = std::common_type_t<long double, ValueType>;
        static constexpr conversion_type value{UnitTraits<DimensionTag, UnitFrom>::scale_to_base / UnitTraits<DimensionTag, UnitTo>::scale_to_base};
    };

    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, typename ValueType>
        requires NumericalType<ValueType>
    struct Quantity
    {
        ValueType value;

        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr Quantity<DimensionTag, TargetUnit, ValueType> convert() const
        {
            using ConversionType = typename ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::conversion_type;
            const auto converted_value = static_cast<ConversionType>(value) * ConversionMap<DimensionTag, Unit, TargetUnit, ValueType>::value;
            return Quantity<DimensionTag, TargetUnit, ValueType>{static_cast<ValueType>(converted_value)};
        }

        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr operator Quantity<DimensionTag, TargetUnit, ValueType>() const
        {
            return convert<TargetUnit>();
        }

        constexpr operator ValueType() const
        {
            return value;
        }

        template <typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType>
        constexpr operator Quantity<DimensionTag, Unit, TargetValueType>() const
        {
            return Quantity<DimensionTag, Unit, TargetValueType>{static_cast<TargetValueType>(value)};
        }

        template <UnitForTag_t<DimensionTag> TargetUnit, typename TargetValueType>
            requires std::is_convertible_v<ValueType, TargetValueType>
        constexpr operator Quantity<DimensionTag, TargetUnit, TargetValueType>() const
        {
            const auto converted_quantity = convert<TargetUnit>();
            return static_cast<Quantity<DimensionTag, TargetUnit, TargetValueType>>(converted_quantity);
        }

        constexpr auto operator<=>(const Quantity &other) const
        {
            return value <=> other.value;
        }

        template <UnitForTag_t<DimensionTag> OtherUnit, typename OtherValueType>
            requires std::three_way_comparable_with<ValueType, OtherValueType>
        constexpr auto operator<=>(const Quantity<DimensionTag, OtherUnit, OtherValueType> &other) const
        {
            const auto converted_other = other.template convert<Unit>();
            using CommonType = std::common_type_t<ValueType, OtherValueType>;
            return static_cast<CommonType>(value) <=> static_cast<CommonType>(converted_other.value);
        }

        friend std::ostream &operator<<(std::ostream &os, const Quantity &q)
        {
            return os << q.value << ' ' << UnitTraits<DimensionTag, Unit>::format_specifier;
        }
    };

    template <SPEED_UNIT Unit, typename ValueType>
    using Speed = Quantity<SpeedTag, Unit, ValueType>;

    template <TIME_UNIT Unit, typename ValueType>
    using Time = Quantity<TimeTag, Unit, ValueType>;

    template <LENGTH_UNIT Unit, typename ValueType>
    using Length = Quantity<LengthTag, Unit, ValueType>;

    using LiteralBase = long double;
    using LiteralBaseInt = long long;

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

    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType>
        requires std::is_invocable_r_v<ValueType, Op, ValueType, ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> apply_binary_op(const Quantity<DimensionTag, LeftUnit, ValueType> &lhs, const Quantity<DimensionTag, LeftUnit, ValueType> &rhs, Op op)
    {
        return Quantity<DimensionTag, LeftUnit, ValueType>{op(lhs.value, rhs.value)};
    }

    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
        requires std::is_invocable_r_v<ValueType, Op, ValueType, ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> apply_binary_op(const Quantity<DimensionTag, LeftUnit, ValueType> &lhs, const Quantity<DimensionTag, RightUnit, ValueType> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<LeftUnit>();
        return Quantity<DimensionTag, LeftUnit, ValueType>{op(lhs.value, rhs_converted.value)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator+(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::plus<ValueType>{});
    }

    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(+)

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return apply_binary_op(a, b, std::minus<ValueType>{});
    }

    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(-)

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType>
    constexpr Quantity<DimensionTag, LeftUnit, ValueType> operator-(const Quantity<DimensionTag, LeftUnit, ValueType> &q)
    {
        return Quantity<DimensionTag, LeftUnit, ValueType>{-q.value};
    }
    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(*)

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator*(const Quantity<DimensionTag, LeftUnit, ValueType> &s, const Scalar a)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(s.value) * static_cast<ResultType>(a)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator*(const Scalar a, const Quantity<DimensionTag, LeftUnit, ValueType> &s)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(a) * static_cast<ResultType>(s.value)};
    }
    DEFINE_MIXED_DIMENSION_OPERATOR_DELETE(/)

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, NumericalType ValueType, NumericalType Scalar>
    constexpr Quantity<DimensionTag, LeftUnit, std::common_type_t<ValueType, Scalar>> operator/(const Quantity<DimensionTag, LeftUnit, ValueType> &s, const Scalar &a)
    {
        using ResultType = std::common_type_t<ValueType, Scalar>;
        return Quantity<DimensionTag, LeftUnit, ResultType>{static_cast<ResultType>(s.value) / static_cast<ResultType>(a)};
    }

#undef DEFINE_MIXED_DIMENSION_OPERATOR_DELETE

    template <typename DimensionTag, UnitForTag_t<DimensionTag> LeftUnit, UnitForTag_t<DimensionTag> RightUnit, NumericalType ValueType>
    constexpr ValueType operator/(const Quantity<DimensionTag, LeftUnit, ValueType> &a, const Quantity<DimensionTag, RightUnit, ValueType> &b)
    {
        return a.value / b.template convert<LeftUnit>().value;
    }

    template <SPEED_UNIT SpeedUnit, TIME_UNIT TimeUnit, NumericalType ValueType>
    constexpr Length<LENGTH_UNIT::M, ValueType> operator*(const Speed<SpeedUnit, ValueType> &speed, const Time<TimeUnit, ValueType> &time)
    {
        const auto speed_ms = speed.template convert<SPEED_UNIT::MS>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Length<LENGTH_UNIT::M, ValueType>{speed_ms.value * time_s.value};
    }

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

    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr bool try_parse_any_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        return (try_parse_unit<DimensionTag, Units>(s, cursor, out) or ...);
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag>... Units>
    constexpr std::optional<ParsedView<DimensionTag>> parse_quantity(std::string_view s)
    {
        std::size_t cursor = 0;
        auto consume_character = [&](char c) constexpr -> bool
        {
            return cursor < s.size() and s[cursor] == c ? (++cursor, true) : false;
        };

        ParsedView<DimensionTag> result{};
        if (!try_parse_any_unit<DimensionTag, Units...>(s, cursor, result))
            return std::nullopt;

        if (auto w = parse_unsigned_at(s, cursor))
            result.width = *w;

        if (consume_character('.'))
        {
            if (auto p = parse_unsigned_at(s, cursor); p)
                result.precision = *p;
            else
                return std::nullopt;

            if (!consume_character('f'))
                return std::nullopt;
        }

        if (cursor != s.size())
            return std::nullopt;

        return result;
    }

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

    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, NumericalType ValueType, UnitForTag_t<DimensionTag>... ValidUnits>
    struct QuantityFormatter : std::formatter<ValueType, char>
    {
        ParsedView<DimensionTag> parsed_format;

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