#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstring>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
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

#define DEFINE_UNIT_TRAITS(_tag, _unit, scale, _suffix, _format_specifier) \
    template <>                                                            \
    struct UnitTraits<_tag, _unit>                                         \
    {                                                                      \
        static constexpr long double scale_to_base{scale};                 \
        static constexpr const char *suffix = _suffix;                     \
        static constexpr const char *format_specifier = _format_specifier; \
    };

    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MS, 1.0L, "ms", "m/s")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KMH, 1000.0L / 3600.0L, "kmh", "km/h")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::MPH, 1609.344L / 3600.0L, "mph", "mi/h")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::KNT, 1852.0L / 3600.0L, "knt", "knt")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_UNIT::C, 299792458.0L, "c", "c")

    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::S, 1.0L, "s", "s")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::MIN, 60.0L, "min", "min")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_UNIT::H, 3600.0L, "h", "h")

    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::M, 1.0L, "m", "m")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::KM, 1000.0L, "km", "km")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::MI, 1609.344L, "mi", "mi")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_UNIT::FT, 0.3048L, "ft", "ft")

#undef DEFINE_UNIT_TRAITS

    template <typename DimensionTag, UnitForTag_t<DimensionTag> UnitFrom, UnitForTag_t<DimensionTag> UnitTo, NumericalType T>
    struct ConversionMap
    {
        static constexpr long double value{UnitTraits<DimensionTag, UnitFrom>::scale_to_base / UnitTraits<DimensionTag, UnitTo>::scale_to_base};
    };

    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, typename T>
        requires NumericalType<T>
    struct Quantity
    {
        T value;

        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr Quantity<DimensionTag, TargetUnit, T> convert() const
        {
            return Quantity<DimensionTag, TargetUnit, T>{static_cast<T>(value * ConversionMap<DimensionTag, Unit, TargetUnit, T>::value)};
        }

        template <UnitForTag_t<DimensionTag> TargetUnit>
        constexpr operator Quantity<DimensionTag, TargetUnit, T>() const
        {
            return convert<TargetUnit>();
        }

        constexpr operator T() const
        {
            return value;
        }

        template <typename U>
            requires std::is_convertible_v<T, U>
        constexpr operator Quantity<DimensionTag, Unit, U>() const
        {
            return Quantity<DimensionTag, Unit, U>{static_cast<U>(value)};
        }

        template <UnitForTag_t<DimensionTag> TargetUnit, typename U>
            requires std::is_convertible_v<T, U>
        constexpr operator Quantity<DimensionTag, TargetUnit, U>() const
        {
            const auto converted_quantity = convert<TargetUnit>();
            return static_cast<Quantity<DimensionTag, TargetUnit, U>>(converted_quantity);
        }

        constexpr auto operator<=>(const Quantity &other) const
        {
            return value <=> other.value;
        }

        template <UnitForTag_t<DimensionTag> OtherUnit, typename U>
            requires std::three_way_comparable_with<T, U>
        constexpr auto operator<=>(const Quantity<DimensionTag, OtherUnit, U> &other) const
        {
            const auto converted_other = other.template convert<Unit>();
            using CommonType = std::common_type_t<T, U>;
            return static_cast<CommonType>(value) <=> static_cast<CommonType>(converted_other.value);
        }

        friend std::ostream &operator<<(std::ostream &os, const Quantity &q)
        {
            return os << q.value << ' ' << UnitTraits<DimensionTag, Unit>::format_specifier;
        }
    };

    template <SPEED_UNIT Unit, typename T>
    using Speed = Quantity<SpeedTag, Unit, T>;

    template <TIME_UNIT Unit, typename T>
    using Time = Quantity<TimeTag, Unit, T>;

    template <LENGTH_UNIT Unit, typename T>
    using Length = Quantity<LengthTag, Unit, T>;

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

    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> A, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Quantity<DimensionTag, A, T> apply_binary_op(const Quantity<DimensionTag, A, T> &lhs, const Quantity<DimensionTag, A, T> &rhs, Op op)
    {
        return Quantity<DimensionTag, A, T>{op(lhs.value, rhs.value)};
    }

    template <typename Op, typename DimensionTag, UnitForTag_t<DimensionTag> A, UnitForTag_t<DimensionTag> B, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Quantity<DimensionTag, A, T> apply_binary_op(const Quantity<DimensionTag, A, T> &lhs, const Quantity<DimensionTag, B, T> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<A>();
        return Quantity<DimensionTag, A, T>{op(lhs.value, rhs_converted.value)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, UnitForTag_t<DimensionTag> B, NumericalType T>
    constexpr Quantity<DimensionTag, A, T> operator+(const Quantity<DimensionTag, A, T> &a, const Quantity<DimensionTag, B, T> &b)
    {
        return apply_binary_op(a, b, std::plus<T>{});
    }

    template <typename DimensionA,
              UnitForTag_t<DimensionA> A,
              NumericalType TA,
              typename DimensionB,
              UnitForTag_t<DimensionB> B,
              NumericalType TB>
        requires(!std::same_as<DimensionA, DimensionB>)
    constexpr void operator+(const Quantity<DimensionA, A, TA> &, const Quantity<DimensionB, B, TB> &) = delete;

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, UnitForTag_t<DimensionTag> B, NumericalType T>
    constexpr Quantity<DimensionTag, A, T> operator-(const Quantity<DimensionTag, A, T> &a, const Quantity<DimensionTag, B, T> &b)
    {
        return apply_binary_op(a, b, std::minus<T>{});
    }

    template <typename DimensionA,
              UnitForTag_t<DimensionA> A,
              NumericalType TA,
              typename DimensionB,
              UnitForTag_t<DimensionB> B,
              NumericalType TB>
        requires(!std::same_as<DimensionA, DimensionB>)
    constexpr void operator-(const Quantity<DimensionA, A, TA> &, const Quantity<DimensionB, B, TB> &) = delete;

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, NumericalType T>
    constexpr Quantity<DimensionTag, A, T> operator-(const Quantity<DimensionTag, A, T> &q)
    {
        return Quantity<DimensionTag, A, T>{-q.value};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, NumericalType T, NumericalType Scalar>
    constexpr Quantity<DimensionTag, A, std::common_type_t<T, Scalar>> operator*(const Quantity<DimensionTag, A, T> &s, const Scalar a)
    {
        using ResultType = std::common_type_t<T, Scalar>;
        return Quantity<DimensionTag, A, ResultType>{static_cast<ResultType>(s.value) * static_cast<ResultType>(a)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, NumericalType T, NumericalType Scalar>
    constexpr Quantity<DimensionTag, A, std::common_type_t<T, Scalar>> operator*(const Scalar a, const Quantity<DimensionTag, A, T> &s)
    {
        using ResultType = std::common_type_t<T, Scalar>;
        return Quantity<DimensionTag, A, ResultType>{static_cast<ResultType>(a) * static_cast<ResultType>(s.value)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, NumericalType T, NumericalType Scalar>
    constexpr Quantity<DimensionTag, A, std::common_type_t<T, Scalar>> operator/(const Quantity<DimensionTag, A, T> &s, const Scalar &a)
    {
        using ResultType = std::common_type_t<T, Scalar>;
        return Quantity<DimensionTag, A, ResultType>{static_cast<ResultType>(s.value) / static_cast<ResultType>(a)};
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> A, UnitForTag_t<DimensionTag> B, NumericalType T>
    constexpr T operator/(const Quantity<DimensionTag, A, T> &a, const Quantity<DimensionTag, B, T> &b)
    {
        return a.value / b.template convert<A>().value;
    }

    template <SPEED_UNIT S, TIME_UNIT Ti, NumericalType T>
    constexpr Quantity<LengthTag, LENGTH_UNIT::M, T> operator*(const Quantity<SpeedTag, S, T> &speed, const Quantity<TimeTag, Ti, T> &time)
    {
        const auto speed_ms = speed.template convert<SPEED_UNIT::MS>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Quantity<LengthTag, LENGTH_UNIT::M, T>{speed_ms.value * time_s.value};
    }

    template <LENGTH_UNIT L, TIME_UNIT Ti, NumericalType T>
    constexpr Quantity<SpeedTag, SPEED_UNIT::MS, T> operator/(const Quantity<LengthTag, L, T> &length, const Quantity<TimeTag, Ti, T> &time)
    {
        const auto length_m = length.template convert<LENGTH_UNIT::M>();
        const auto time_s = time.template convert<TIME_UNIT::S>();
        return Quantity<SpeedTag, SPEED_UNIT::MS, T>{length_m.value / time_s.value};
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

    template <typename DimensionTag, UnitForTag_t<DimensionTag> R>
    constexpr bool try_parse_unit(std::string_view s, std::size_t &cursor, ParsedView<DimensionTag> &out)
    {
        const auto suffix = UnitTraits<DimensionTag, R>::suffix;
        if (!s.substr(cursor).starts_with(suffix))
            return false;

        out.unit = UnitTraits<DimensionTag, R>::format_specifier;
        out.target_unit = R;
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

    template <typename DimensionTag>
    constexpr const char *formatter_error_message();

    template <>
    constexpr const char *formatter_error_message<SpeedTag>()
    {
        return "invalid format specifier for Speed. Use 'ms', 'kmh', 'knt', 'c', or 'mph', optionally followed by a number with an 'f' suffix (e.g., 'kmh1.5f').";
    }

    template <>
    constexpr const char *formatter_error_message<TimeTag>()
    {
        return "invalid format specifier for Time. Use 's', 'min', or 'h', optionally followed by a number with an 'f' suffix (e.g., 'min8.2f').";
    }

    template <>
    constexpr const char *formatter_error_message<LengthTag>()
    {
        return "invalid format specifier for Length. Use 'm', 'km', 'mi', or 'ft', optionally followed by a number with an 'f' suffix (e.g., 'km8.2f').";
    }

    template <typename DimensionTag, UnitForTag_t<DimensionTag> Unit, NumericalType T, UnitForTag_t<DimensionTag>... ValidUnits>
    struct QuantityFormatter : std::formatter<T, char>
    {
        ParsedView<DimensionTag> parsed_format;

        template <UnitForTag_t<DimensionTag> CandidateUnit, UnitForTag_t<DimensionTag>... RemainingUnits>
        static constexpr void convert_if_unit_matches(
            UnitForTag_t<DimensionTag> target_unit,
            const Quantity<DimensionTag, Unit, T> &quantity,
            T &formatted_value)
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
                throw std::format_error(formatter_error_message<DimensionTag>());

            return it;
        }

        template <typename FormatContext>
        auto format(const Quantity<DimensionTag, Unit, T> &quantity, FormatContext &ctx) const
        {
            const auto unit = parsed_format.unit != nullptr
                                  ? parsed_format.unit
                                  : UnitTraits<DimensionTag, Unit>::format_specifier;

            T formatted_value = quantity.value;
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

template <speed_lib::SPEED_UNIT r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Speed<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::SpeedTag,
          r,
          T,
          speed_lib::SPEED_UNIT::MS,
          speed_lib::SPEED_UNIT::KMH,
          speed_lib::SPEED_UNIT::MPH,
          speed_lib::SPEED_UNIT::KNT,
          speed_lib::SPEED_UNIT::C>
{
};

template <speed_lib::TIME_UNIT r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Time<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::TimeTag,
          r,
          T,
          speed_lib::TIME_UNIT::S,
          speed_lib::TIME_UNIT::MIN,
          speed_lib::TIME_UNIT::H>
{
};

template <speed_lib::LENGTH_UNIT r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Length<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::LengthTag,
          r,
          T,
          speed_lib::LENGTH_UNIT::KM,
          speed_lib::LENGTH_UNIT::MI,
          speed_lib::LENGTH_UNIT::FT,
          speed_lib::LENGTH_UNIT::M>
{
};