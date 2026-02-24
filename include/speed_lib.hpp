#include <concepts>
#include <type_traits>
#include <format>
#include <functional>
#include <iostream>
#include <cstring>

namespace speed_lib
{
    enum class SPEED_REPRESENTATION : char
    {
        MS,  //<= meters per second
        KMH, //<= kilometers per hour
        MPH, //<= miles per hour
        KNT, //<= knots
        C    //<= speed of light
    };

    template <typename T>
    concept NumericalType = std::is_arithmetic_v<T>;
    constexpr SPEED_REPRESENTATION DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
    using LiteralBase = long double;

    template <SPEED_REPRESENTATION>
    struct UnitScaleToMs;

#define DEFINE_UNIT_SCALE_TO_MS(unit, scale)         \
    template <>                                      \
    struct UnitScaleToMs<SPEED_REPRESENTATION::unit> \
    {                                                \
        static constexpr long double value{scale};   \
    };

    DEFINE_UNIT_SCALE_TO_MS(MS, 1.0L)
    DEFINE_UNIT_SCALE_TO_MS(KMH, 1000.0L / 3600.0L)
    DEFINE_UNIT_SCALE_TO_MS(MPH, 1609.344L / 3600.0L)
    DEFINE_UNIT_SCALE_TO_MS(KNT, 1852.0L / 3600.0L)
    DEFINE_UNIT_SCALE_TO_MS(C, 299792458.0L)
#undef DEFINE_UNIT_SCALE_TO_MS

    template <SPEED_REPRESENTATION FROM, SPEED_REPRESENTATION TO, NumericalType T>
    struct SpeedConversionMap
    {
        static constexpr T value{static_cast<T>(UnitScaleToMs<FROM>::value / UnitScaleToMs<TO>::value)};
    };

    template <SPEED_REPRESENTATION A>
    struct SpeedLiteralMap;

#define DEFINE_LITERAL_MAP(unit, _suffix, _format_specifier)               \
    template <>                                                            \
    struct SpeedLiteralMap<SPEED_REPRESENTATION::unit>                     \
    {                                                                      \
        static constexpr const char *suffix = _suffix;                     \
        static constexpr const char *format_specifier = _format_specifier; \
    };

    DEFINE_LITERAL_MAP(MS, "ms", "m/s")
    DEFINE_LITERAL_MAP(KMH, "kmh", "km/h")
    DEFINE_LITERAL_MAP(MPH, "mph", "mi/h")
    DEFINE_LITERAL_MAP(KNT, "knt", "knt")
    DEFINE_LITERAL_MAP(C, "c", "c")
#undef DEFINE_LITERAL_MAP

    // Value is tagged with its representation to allow for implicit conversions and operator overloads that handle different representations.
    template <SPEED_REPRESENTATION U, typename T>
        requires NumericalType<T>
    struct Speed
    {
        T value;

        // Convert speed to another representation
        template <SPEED_REPRESENTATION OUT>
        constexpr Speed<OUT, T> convert() const
        {
            return Speed<OUT, T>{value * SpeedConversionMap<U, OUT, T>::value};
        }

        template <SPEED_REPRESENTATION OUT>
        constexpr operator Speed<OUT, T>() const
        {
            return convert<OUT>();
        }

        // Convert enclosed value to another type
        template <typename V>
            requires std::is_convertible_v<V, T>
        constexpr operator Speed<U, V>() const
        {
            return Speed<U, V>{static_cast<V>(value)};
        }

        // Convert both representation and value type
        template <SPEED_REPRESENTATION OUT, typename V>
            requires std::is_convertible_v<V, T>
        constexpr operator Speed<OUT, V>() const
        {
            const auto converted_speed = convert<OUT>();
            return static_cast<Speed<OUT, V>>(converted_speed);
        }

        // std::cout and so on
        friend std::ostream &operator<<(std::ostream &os, const Speed &s)
        {
            return os << std::format("{}", s);
        }
    };

#define DEFINE_LITERAL_OPERATOR(unit, suffix)                                                       \
    constexpr Speed<SPEED_REPRESENTATION::unit, LiteralBase> operator""_##suffix(LiteralBase value) \
    {                                                                                               \
        return Speed<SPEED_REPRESENTATION::unit, LiteralBase>{value};                               \
    }

    DEFINE_LITERAL_OPERATOR(MS, ms)
    DEFINE_LITERAL_OPERATOR(KMH, kmh)
    DEFINE_LITERAL_OPERATOR(MPH, mph)
    DEFINE_LITERAL_OPERATOR(KNT, knt)
    DEFINE_LITERAL_OPERATOR(C, c)
#undef DEFINE_LITERAL_OPERATOR
    // The whole reason this library exists is to add units
    // This is why the overloads represent proper unit-aware operations instead of just converting to a common representation and doing the operation there.

    // C[unit] = A[unit] + B[unit]
    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, NumericalType T>
    constexpr Speed<A, T> operator+(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::plus<T>{});
    }

    // C[unit] = A[unit] - B[unit]
    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, NumericalType T>
    constexpr Speed<A, T> operator-(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::minus<T>{});
    }

    // A[unit] = A[unit] * scalar
    template <SPEED_REPRESENTATION A, NumericalType T>
    constexpr Speed<A, T> operator*(const Speed<A, T> &s, const T a)
    {
        return Speed<A, T>{s.value * a};
    }

    // A[unit] = scalar * A[unit]
    template <SPEED_REPRESENTATION A, NumericalType T>
    constexpr Speed<A, T> operator*(const T a, const Speed<A, T> &s)
    {
        return Speed<A, T>{a * s.value};
    }

    // A[unit] = A[unit] / scalar. We have no way to invert units, so we can't do scalar / A[unit]
    template <SPEED_REPRESENTATION A, NumericalType T>
    constexpr Speed<A, T> operator/(const Speed<A, T> &s, const T &a)
    {
        return Speed<A, T>{s.value / a};
    }

    // scalar = A[unit] / A[unit]
    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, NumericalType T>
    constexpr T operator/(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return a.value / b.template convert<A>().value;
    }

    template <typename Op, SPEED_REPRESENTATION A, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<A, T> &rhs, Op op)
    {
        return Speed<A, T>{op(lhs.value, rhs.value)};
    }

    template <typename Op, SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, NumericalType T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<B, T> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<A>();
        return Speed<A, T>{op(lhs.value, rhs_converted.value)};
    }

    struct ParsedView
    {
        const char *unit;
        std::optional<unsigned> width = std::nullopt;
        std::optional<unsigned> precision = std::nullopt;
    };

    constexpr bool is_digit(char c) { return c >= '0' and c <= '9'; }

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

    template <SPEED_REPRESENTATION R>
    constexpr bool try_parse_unit(std::string_view s, std::size_t &cursor, ParsedView &out)
    {
        const auto suffix = SpeedLiteralMap<R>::suffix;
        if (!s.substr(cursor).starts_with(suffix))
            return false;

        out.unit = SpeedLiteralMap<R>::format_specifier;
        cursor += std::strlen(suffix);
        return true;
    }

    // Grammar: (ms|kmh|mph|knt|c)([0-9]+)?(\.[0-9]+f)? why is there no constexpr regex in C++23 FML
    constexpr std::optional<ParsedView> parse_speed(std::string_view s)
    {
        std::size_t cursor = 0;

        auto consumeCharacter = [&](char c) constexpr -> bool
        {
            return cursor < s.size() and s[cursor] == c ? (++cursor, true) : false;
        };

        ParsedView result{};

        // unit
        if (!(try_parse_unit<SPEED_REPRESENTATION::MS>(s, cursor, result) or
              try_parse_unit<SPEED_REPRESENTATION::KMH>(s, cursor, result) or
              try_parse_unit<SPEED_REPRESENTATION::MPH>(s, cursor, result) or
              try_parse_unit<SPEED_REPRESENTATION::KNT>(s, cursor, result) or
              try_parse_unit<SPEED_REPRESENTATION::C>(s, cursor, result)))
        {
            return std::nullopt;
        }

        // optional width digits
        if (auto w = parse_unsigned_at(s, cursor))
            result.width = *w;

        // optional precision: '.' digits+ 'f'
        if (consumeCharacter('.'))
        {
            auto p = parse_unsigned_at(s, cursor);
            if (!p)
                return std::nullopt;
            if (!consumeCharacter('f'))
                return std::nullopt;
            result.precision = *p;
        }

        if (cursor != s.size())
            return std::nullopt;
        return result;
    }
}

// this needs to be inside of the std namespace to be picked up by std::format
template <speed_lib::SPEED_REPRESENTATION r, speed_lib::NumericalType T>
struct std::formatter<speed_lib::Speed<r, T>> : std::formatter<T, char>
{
    speed_lib::ParsedView parsed_format;
    constexpr auto parse(std::format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end or *it == '}')
        {
            parsed_format = speed_lib::ParsedView{speed_lib::SpeedLiteralMap<speed_lib::DefaultSpeedRepresentation>::format_specifier, {}, {}};
            return it;
        }

        auto start = it;
        while (it != end and *it != '}')
            ++it;

        std::string_view spec{&*start, static_cast<size_t>(it - start)};
        if (auto parsed = speed_lib::parse_speed(spec); parsed.has_value())
            parsed_format = *parsed;
        else
            throw std::format_error("invalid format specifier for Speed. Use 'ms', 'kmh', 'knt', 'c', or 'mph', optionally followed by a number with an 'f' suffix (e.g., 'kmh1.5f').");

        return it;
    }

    template <typename FormatContext>
    auto format(const speed_lib::Speed<r, T> &s, FormatContext &ctx) const
    {
        if (parsed_format.width and parsed_format.precision)
        {
            std::format_to(ctx.out(), "{:>{}.{}f}", s.value, *parsed_format.width, *parsed_format.precision);
        }
        else if (parsed_format.width)
        {
            std::format_to(ctx.out(), "{:>{}}", s.value, *parsed_format.width);
        }
        else if (parsed_format.precision)
        {
            std::format_to(ctx.out(), "{:.{}f}", s.value, *parsed_format.precision);
        }
        else
        {
            std::format_to(ctx.out(), "{}", s.value);
        }
        return std::format_to(ctx.out(), " {}", parsed_format.unit);
    }
};