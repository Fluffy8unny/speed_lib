#include <concepts>
#include <type_traits>
#include <format>
#include <functional>
#include <iostream>

namespace speed_lib
{
    enum class SPEED_REPRESENTATION : char
    {
        MS,  //<= meters per second
        KMH, //<= kilometers per hour
        MPH  //<= miles per hour
    };

    template <typename T>
    concept Number = std::is_arithmetic_v<T>;
    constexpr SPEED_REPRESENTATION DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
    using LiteralBase = long double;

    template <SPEED_REPRESENTATION, SPEED_REPRESENTATION, Number>
    struct SpeedConversionMap;

    constexpr double MS_TO_KMH = 3.6;
    constexpr double MPH_TO_KMH = 1.60934;
    constexpr double MS_TO_MPH = 2.23694;

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::MS, SPEED_REPRESENTATION::KMH, T>
    {
        static constexpr T value{MS_TO_KMH};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::MS, SPEED_REPRESENTATION::MPH, T>
    {
        static constexpr T value{MS_TO_MPH};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::KMH, SPEED_REPRESENTATION::MS, T>
    {
        static constexpr T value{1.0 / MS_TO_KMH};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::KMH, SPEED_REPRESENTATION::MPH, T>
    {
        static constexpr T value{1.0 / MPH_TO_KMH};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::MPH, SPEED_REPRESENTATION::MS, T>
    {
        static constexpr T value{1.0 / MS_TO_MPH};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::MPH, SPEED_REPRESENTATION::KMH, T>
    {
        static constexpr T value{MPH_TO_KMH};
    };

    template <SPEED_REPRESENTATION A>
    struct SpeedLiteralMap;

    template <>
    struct SpeedLiteralMap<SPEED_REPRESENTATION::MS>
    {
        static constexpr const char *suffix = "ms";
        static constexpr const char *format_specifier = "m/s";
    };

    template <>
    struct SpeedLiteralMap<SPEED_REPRESENTATION::KMH>
    {
        static constexpr const char *suffix = "kmh";
        static constexpr const char *format_specifier = "km/h";
    };

    template <>
    struct SpeedLiteralMap<SPEED_REPRESENTATION::MPH>
    {
        static constexpr const char *suffix = "mph";
        static constexpr const char *format_specifier = "mi/h";
    };

    // Value is tagged with its representation to allow for implicit conversions and operator overloads that handle different representations.
    template <SPEED_REPRESENTATION U, typename T>
        requires Number<T>
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

    constexpr Speed<SPEED_REPRESENTATION::MS, LiteralBase> operator""_ms(LiteralBase value)
    {
        return Speed<SPEED_REPRESENTATION::MS, LiteralBase>{value};
    }

    constexpr Speed<SPEED_REPRESENTATION::KMH, LiteralBase> operator""_kmh(LiteralBase value)
    {
        return Speed<SPEED_REPRESENTATION::KMH, LiteralBase>{value};
    }

    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
    constexpr Speed<A, T> operator+(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::plus<T>{});
    }

    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
    constexpr Speed<A, T> operator-(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::minus<T>{});
    }

    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
    constexpr Speed<A, T> operator*(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::multiplies<T>{});
    }

    template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
    constexpr Speed<A, T> operator/(const Speed<A, T> &a, const Speed<B, T> &b)
    {
        return apply_binary_op(a, b, std::divides<T>{});
    }

    template <SPEED_REPRESENTATION A, Number T>
        requires std::is_signed_v<T>
    constexpr Speed<A, T> operator-(const Speed<A, T> &s)
    {
        return Speed<A, T>{-s.value};
    }

    template <typename Op, SPEED_REPRESENTATION A, Number T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<A, T> &rhs, Op op)
    {
        return Speed<A, T>{op(lhs.value, rhs.value)};
    }

    template <typename Op, SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
        requires std::is_invocable_r_v<T, Op, T, T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<B, T> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<A>();
        return Speed<A, T>{op(lhs.value, rhs_converted.value)};
    }

    struct ParsedView
    {
        const char *unit;
        std::optional<unsigned> width;
        std::optional<unsigned> precision;
    };
    constexpr bool is_digit(char c) { return c >= '0' && c <= '9'; }

    constexpr std::optional<unsigned> parse_unsigned_at(std::string_view s, std::size_t &i)
    {
        if (i >= s.size() || !is_digit(s[i]))
            return std::nullopt;

        unsigned v = 0;
        while (i < s.size() && is_digit(s[i]))
        {
            v = v * 10u + static_cast<unsigned>(s[i] - '0');
            ++i;
        }
        return v;
    }

    // Grammar: (ms|kmh|mph)([0-9]+)?(\.[0-9]+f)? why is there no constexpr regex in C++23 FML
    constexpr std::optional<ParsedView> parse_speed(std::string_view s)
    {
        std::size_t cursor = 0;

        auto take = [&](char c) constexpr -> bool
        {
            return cursor < s.size() && s[cursor] == c ? (++cursor, true) : false;
        };

        ParsedView out{};
        out.width = std::nullopt;
        out.precision = std::nullopt;

        // unit
        if (s.substr(cursor).starts_with(SpeedLiteralMap<SPEED_REPRESENTATION::MS>::suffix))
        {
            out.unit = SpeedLiteralMap<SPEED_REPRESENTATION::MS>::format_specifier;
            cursor += 2;
        }
        else if (s.substr(cursor).starts_with(SpeedLiteralMap<SPEED_REPRESENTATION::KMH>::suffix))
        {
            out.unit = SpeedLiteralMap<SPEED_REPRESENTATION::KMH>::format_specifier;
            cursor += 3;
        }
        else if (s.substr(cursor).starts_with(SpeedLiteralMap<SPEED_REPRESENTATION::MPH>::suffix))
        {
            out.unit = SpeedLiteralMap<SPEED_REPRESENTATION::MPH>::format_specifier;
            cursor += 3;
        }
        else
        {
            return std::nullopt;
        }

        // optional width digits
        if (auto w = parse_unsigned_at(s, cursor))
            out.width = *w;

        // optional precision: '.' digits+ 'f'
        if (take('.'))
        {
            auto p = parse_unsigned_at(s, cursor);
            if (!p)
                return std::nullopt;
            if (!take('f'))
                return std::nullopt;
            out.precision = *p;
        }

        if (cursor != s.size())
            return std::nullopt;
        return out;
    }

}

// this needs to be inside of the std namespace to be picked up by std::format
template <speed_lib::SPEED_REPRESENTATION r, speed_lib::Number T>
struct std::formatter<speed_lib::Speed<r, T>> : std::formatter<T, char>
{
    speed_lib::ParsedView parsed_format;
    constexpr auto parse(std::format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end || *it == '}')
        {
            parsed_format = speed_lib::ParsedView{speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::MS>::format_specifier, {}, {}};
            return it;
        }

        auto start = it;
        while (it != end && *it != '}')
            ++it;

        std::string_view spec{&*start, static_cast<size_t>(it - start)};
        if (auto parsed = speed_lib::parse_speed(spec); parsed.has_value())

            parsed_format = *parsed;
        else
            throw std::format_error("invalid format specifier for Speed. Use 'ms', 'kmh', or 'mph', optionally followed by a number with an 'f' suffix (e.g., 'kmh1.5f').");

        return it;
    }

    template <typename FormatContext>
    auto format(const speed_lib::Speed<r, T> &s, FormatContext &ctx) const
    {
        if (parsed_format.width && parsed_format.precision)
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