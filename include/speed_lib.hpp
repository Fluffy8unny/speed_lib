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

    struct SpeedTag
    {
    };

    struct TimeTag
    {
    };

    struct LengthTag
    {
    };

    template <typename Tag>
    struct RepresentationForTag;

    enum class SPEED_REPRESENTATION : char
    {
        MS,
        KMH,
        MPH,
        KNT,
        C
    };

    enum class TIME_REPRESENTATION : char
    {
        S,
        MIN,
        H
    };

    enum class LENGTH_REPRESENTATION : char
    {
        M,
        KM,
        MI,
        FT
    };

    template <>
    struct RepresentationForTag<SpeedTag>
    {
        using type = SPEED_REPRESENTATION;
    };

    template <>
    struct RepresentationForTag<TimeTag>
    {
        using type = TIME_REPRESENTATION;
    };

    template <>
    struct RepresentationForTag<LengthTag>
    {
        using type = LENGTH_REPRESENTATION;
    };

    template <typename Tag>
    using RepresentationForTag_t = typename RepresentationForTag<Tag>::type;

    template <typename Tag, auto Unit>
    concept TagRepresentationMatch = std::same_as<std::remove_cv_t<decltype(Unit)>, RepresentationForTag_t<Tag>>;

#define SPEED_REPRESENTATION_TEMPLATE_LIST \
    SPEED_REPRESENTATION::MS,              \
        SPEED_REPRESENTATION::KMH,         \
        SPEED_REPRESENTATION::MPH,         \
        SPEED_REPRESENTATION::KNT,         \
        SPEED_REPRESENTATION::C

#define TIME_REPRESENTATION_TEMPLATE_LIST \
    TIME_REPRESENTATION::S,               \
        TIME_REPRESENTATION::MIN,         \
        TIME_REPRESENTATION::H

#define LENGTH_REPRESENTATION_TEMPLATE_LIST \
    LENGTH_REPRESENTATION::M,               \
        LENGTH_REPRESENTATION::KM,          \
        LENGTH_REPRESENTATION::MI,          \
        LENGTH_REPRESENTATION::FT

    constexpr SPEED_REPRESENTATION DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
    constexpr TIME_REPRESENTATION DefaultTimeRepresentation = TIME_REPRESENTATION::S;
    constexpr LENGTH_REPRESENTATION DefaultLengthRepresentation = LENGTH_REPRESENTATION::M;

    using LiteralBase = long double;

    template <typename Tag, auto Unit>
    struct UnitTraits;

#define DEFINE_UNIT_TRAITS(tag, unit, scale, _suffix, _format_specifier)   \
    template <>                                                            \
    struct UnitTraits<tag, unit>                                           \
    {                                                                      \
        static constexpr long double scale_to_base{scale};                 \
        static constexpr const char *suffix = _suffix;                     \
        static constexpr const char *format_specifier = _format_specifier; \
    };

    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_REPRESENTATION::MS, 1.0L, "ms", "m/s")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_REPRESENTATION::KMH, 1000.0L / 3600.0L, "kmh", "km/h")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_REPRESENTATION::MPH, 1609.344L / 3600.0L, "mph", "mi/h")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_REPRESENTATION::KNT, 1852.0L / 3600.0L, "knt", "knt")
    DEFINE_UNIT_TRAITS(SpeedTag, SPEED_REPRESENTATION::C, 299792458.0L, "c", "c")

    DEFINE_UNIT_TRAITS(TimeTag, TIME_REPRESENTATION::S, 1.0L, "s", "s")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_REPRESENTATION::MIN, 60.0L, "min", "min")
    DEFINE_UNIT_TRAITS(TimeTag, TIME_REPRESENTATION::H, 3600.0L, "h", "h")

    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_REPRESENTATION::M, 1.0L, "m", "m")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_REPRESENTATION::KM, 1000.0L, "km", "km")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_REPRESENTATION::MI, 1609.344L, "mi", "mi")
    DEFINE_UNIT_TRAITS(LengthTag, LENGTH_REPRESENTATION::FT, 0.3048L, "ft", "ft")

#undef DEFINE_UNIT_TRAITS

    template <typename Tag, auto FROM, auto TO, NumericalType T>
    struct ConversionMap
    {
        static constexpr T value{static_cast<T>(UnitTraits<Tag, FROM>::scale_to_base / UnitTraits<Tag, TO>::scale_to_base)};
    };

    template <typename Tag, auto U, typename T>
        requires NumericalType<T> && TagRepresentationMatch<Tag, U>
    struct Quantity
    {
        T value;

        template <auto OUT>
        constexpr Quantity<Tag, OUT, T> convert() const
        {
            return Quantity<Tag, OUT, T>{value * ConversionMap<Tag, U, OUT, T>::value};
        }

        template <auto OUT>
        constexpr operator Quantity<Tag, OUT, T>() const
        {
            return convert<OUT>();
        }

        constexpr operator T() const
        {
            return value;
        }

        template <typename V>
            requires std::is_convertible_v<T, V>
        constexpr operator Quantity<Tag, U, V>() const
        {
            return Quantity<Tag, U, V>{static_cast<V>(value)};
        }

        template <auto OUT, typename V>
            requires std::is_convertible_v<T, V>
        constexpr operator Quantity<Tag, OUT, V>() const
        {
            const auto converted_quantity = convert<OUT>();
            return static_cast<Quantity<Tag, OUT, V>>(converted_quantity);
        }
        
        constexpr auto operator<=>(const Quantity &other) const
        {
            return value <=> other.value;
        }

        template <auto OTHER_U, typename V>
            requires std::three_way_comparable_with<T, V>
        constexpr auto operator<=>(const Quantity<Tag, OTHER_U, V> &other) const
        {
            const auto converted_other = other.template convert<U>();
            using CommonType = std::common_type_t<T, V>;
            return static_cast<CommonType>(value) <=> static_cast<CommonType>(converted_other.value);
        }

        friend std::ostream &operator<<(std::ostream &os, const Quantity &q)
        {
            return os << q.value << ' ' << UnitTraits<Tag, U>::format_specifier;
        }
    };

    template <SPEED_REPRESENTATION U, typename T>
    using Speed = Quantity<SpeedTag, U, T>;

    template <TIME_REPRESENTATION U, typename T>
    using Time = Quantity<TimeTag, U, T>;

    template <LENGTH_REPRESENTATION U, typename T>
    using Length = Quantity<LengthTag, U, T>;

    template <SPEED_REPRESENTATION U>
    using SpeedLiteralMap = UnitTraits<SpeedTag, U>;

#define DEFINE_LITERAL_OPERATOR(TagType, RepresentationType, unit, suffix)                            \
    constexpr Quantity<TagType, RepresentationType::unit, LiteralBase> operator""_##suffix(LiteralBase value) \
    {                                                                                                         \
        return Quantity<TagType, RepresentationType::unit, LiteralBase>{value};                               \
    }

    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_REPRESENTATION, MS, ms)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_REPRESENTATION, KMH, kmh)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_REPRESENTATION, MPH, mph)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_REPRESENTATION, KNT, knt)
    DEFINE_LITERAL_OPERATOR(SpeedTag, SPEED_REPRESENTATION, C, c)

    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_REPRESENTATION, S, s)
    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_REPRESENTATION, MIN, min)
    DEFINE_LITERAL_OPERATOR(TimeTag, TIME_REPRESENTATION, H, h)

    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_REPRESENTATION, M, m)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_REPRESENTATION, KM, km)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_REPRESENTATION, MI, mi)
    DEFINE_LITERAL_OPERATOR(LengthTag, LENGTH_REPRESENTATION, FT, ft)
#undef DEFINE_LITERAL_OPERATOR

    template <typename Op, typename Tag, auto A, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Quantity<Tag, A, T> apply_binary_op(const Quantity<Tag, A, T> &lhs, const Quantity<Tag, A, T> &rhs, Op op)
    {
        return Quantity<Tag, A, T>{op(lhs.value, rhs.value)};
    }

    template <typename Op, typename Tag, auto A, auto B, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Quantity<Tag, A, T> apply_binary_op(const Quantity<Tag, A, T> &lhs, const Quantity<Tag, B, T> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<A>();
        return Quantity<Tag, A, T>{op(lhs.value, rhs_converted.value)};
    }

    template <typename Tag, auto A, auto B, NumericalType T>
    constexpr Quantity<Tag, A, T> operator+(const Quantity<Tag, A, T> &a, const Quantity<Tag, B, T> &b)
    {
        return apply_binary_op(a, b, std::plus<T>{});
    }

    template <typename Tag, auto A, auto B, NumericalType T>
    constexpr Quantity<Tag, A, T> operator-(const Quantity<Tag, A, T> &a, const Quantity<Tag, B, T> &b)
    {
        return apply_binary_op(a, b, std::minus<T>{});
    }

    template <typename Tag, auto A, NumericalType T>
    constexpr Quantity<Tag, A, T> operator*(const Quantity<Tag, A, T> &s, const T a)
    {
        return Quantity<Tag, A, T>{s.value * a};
    }

    template <typename Tag, auto A, NumericalType T>
    constexpr Quantity<Tag, A, T> operator*(const T a, const Quantity<Tag, A, T> &s)
    {
        return Quantity<Tag, A, T>{a * s.value};
    }

    template <SPEED_REPRESENTATION S, TIME_REPRESENTATION Ti, NumericalType T>
    constexpr Quantity<LengthTag, LENGTH_REPRESENTATION::M, T> operator*(const Quantity<SpeedTag, S, T> &speed, const Quantity<TimeTag, Ti, T> &time)
    {
        const auto speed_ms = speed.template convert<SPEED_REPRESENTATION::MS>();
        const auto time_s = time.template convert<TIME_REPRESENTATION::S>();
        return Quantity<LengthTag, LENGTH_REPRESENTATION::M, T>{speed_ms.value * time_s.value};
    }

    template <LENGTH_REPRESENTATION L, TIME_REPRESENTATION Ti, NumericalType T>
    constexpr Quantity<SpeedTag, SPEED_REPRESENTATION::MS, T> operator/(const Quantity<LengthTag, L, T> &length, const Quantity<TimeTag, Ti, T> &time)
    {
        const auto length_m = length.template convert<LENGTH_REPRESENTATION::M>();
        const auto time_s = time.template convert<TIME_REPRESENTATION::S>();
        return Quantity<SpeedTag, SPEED_REPRESENTATION::MS, T>{length_m.value / time_s.value};
    }

    template <typename Tag, auto A, NumericalType T>
    constexpr Quantity<Tag, A, T> operator/(const Quantity<Tag, A, T> &s, const T &a)
    {
        return Quantity<Tag, A, T>{s.value / a};
    }

    template <typename Tag, auto A, auto B, NumericalType T>
    constexpr T operator/(const Quantity<Tag, A, T> &a, const Quantity<Tag, B, T> &b)
    {
        return a.value / b.template convert<A>().value;
    }

    struct ParsedView
    {
        const char *unit;
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

    template <typename Tag, auto R>
    constexpr bool try_parse_unit(std::string_view s, std::size_t &cursor, ParsedView &out)
    {
        const auto suffix = UnitTraits<Tag, R>::suffix;
        if (!s.substr(cursor).starts_with(suffix))
            return false;

        out.unit = UnitTraits<Tag, R>::format_specifier;
        cursor += std::strlen(suffix);
        return true;
    }

    template <typename Tag, auto... Units>
    constexpr bool try_parse_any_unit(std::string_view s, std::size_t &cursor, ParsedView &out)
    {
        return (try_parse_unit<Tag, Units>(s, cursor, out) or ...);
    }

    template <typename Tag, auto... Units>
    constexpr std::optional<ParsedView> parse_quantity(std::string_view s)
    {
        std::size_t cursor = 0;
        auto consume_character = [&](char c) constexpr -> bool
        {
            return cursor < s.size() and s[cursor] == c ? (++cursor, true) : false;
        };

        ParsedView result{};
        if (!try_parse_any_unit<Tag, Units...>(s, cursor, result))
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

    constexpr std::optional<ParsedView> parse_speed(std::string_view s)
    {
        return parse_quantity<SpeedTag, SPEED_REPRESENTATION_TEMPLATE_LIST>(s);
    }

    constexpr std::optional<ParsedView> parse_time(std::string_view s)
    {
        return parse_quantity<TimeTag, TIME_REPRESENTATION_TEMPLATE_LIST>(s);
    }

    constexpr std::optional<ParsedView> parse_length(std::string_view s)
    {
        return parse_quantity<LengthTag, LENGTH_REPRESENTATION_TEMPLATE_LIST>(s);
    }

    template <typename Tag>
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

    template <typename Tag, auto Representation, auto DefaultRepresentation, NumericalType T, auto... ValidRepresentations>
    struct QuantityFormatter : std::formatter<T, char>
    {
        ParsedView parsed_format;

        constexpr auto parse(std::format_parse_context &ctx)
        {
            auto it = ctx.begin();
            auto end = ctx.end();

            if (it == end or *it == '}')
            {
                parsed_format = ParsedView{UnitTraits<Tag, DefaultRepresentation>::format_specifier, {}, {}};
                return it;
            }

            auto start = it;
            it = std::find(it, end, '}');

            std::string_view spec{&*start, static_cast<size_t>(it - start)};
            if (auto parsed = parse_quantity<Tag, ValidRepresentations...>(spec); parsed.has_value())
                parsed_format = *parsed;
            else
                throw std::format_error(formatter_error_message<Tag>());

            return it;
        }

        template <typename FormatContext>
        auto format(const Quantity<Tag, Representation, T> &quantity, FormatContext &ctx) const
        {
            if (parsed_format.width and parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:>{}.{}f}", quantity.value, *parsed_format.width, *parsed_format.precision);
            }
            else if (parsed_format.width)
            {
                std::format_to(ctx.out(), "{:>{}}", quantity.value, *parsed_format.width);
            }
            else if (parsed_format.precision)
            {
                std::format_to(ctx.out(), "{:.{}f}", quantity.value, *parsed_format.precision);
            }
            else
            {
                std::format_to(ctx.out(), "{}", quantity.value);
            }

            return std::format_to(ctx.out(), " {}", parsed_format.unit);
        }
    };
}

template <speed_lib::SPEED_REPRESENTATION r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Speed<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::SpeedTag,
          r,
          speed_lib::DefaultSpeedRepresentation,
          T,
          speed_lib::SPEED_REPRESENTATION::MS,
          speed_lib::SPEED_REPRESENTATION::KMH,
          speed_lib::SPEED_REPRESENTATION::MPH,
          speed_lib::SPEED_REPRESENTATION::KNT,
          speed_lib::SPEED_REPRESENTATION::C>
{
};

template <speed_lib::TIME_REPRESENTATION r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Time<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::TimeTag,
          r,
          speed_lib::DefaultTimeRepresentation,
          T,
          speed_lib::TIME_REPRESENTATION::S,
          speed_lib::TIME_REPRESENTATION::MIN,
          speed_lib::TIME_REPRESENTATION::H>
{
};

template <speed_lib::LENGTH_REPRESENTATION r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Length<r, T>>
    : speed_lib::QuantityFormatter<
          speed_lib::LengthTag,
          r,
          speed_lib::DefaultLengthRepresentation,
          T,
          speed_lib::LENGTH_REPRESENTATION::M,
          speed_lib::LENGTH_REPRESENTATION::KM,
          speed_lib::LENGTH_REPRESENTATION::MI,
          speed_lib::LENGTH_REPRESENTATION::FT>
{
};