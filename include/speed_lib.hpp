#include <concepts>
#include <type_traits>
#include <format>
#include <functional>

namespace speed_lib
{
    enum class SPEED_REPRESENTATION : char
    {
        MS, //<= meters per second
        KMH //<= kilometers per hour
    };

    template <typename T>
    concept Number = std::is_arithmetic_v<T>;
    constexpr SPEED_REPRESENTATION DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
    using LiteralBase = long double;

    template <SPEED_REPRESENTATION, SPEED_REPRESENTATION, Number>
    struct SpeedConversionMap;

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::MS, SPEED_REPRESENTATION::KMH, T>
    {
        static constexpr T value{3.6};
    };

    template <Number T>
    struct SpeedConversionMap<SPEED_REPRESENTATION::KMH, SPEED_REPRESENTATION::MS, T>
    {
        static constexpr T value{1.0 / 3.6};
    };

    template <SPEED_REPRESENTATION U, typename T>
        requires Number<T>
    struct Speed
    {
        T value;
     
        //Convert speed to another representation
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

        //Convert enclosed value to another type
        template <typename V>
            requires std::is_convertible_v<V, T>
        constexpr operator Speed<U, V>() const
        {
            return Speed<U, V>{static_cast<V>(value)};
        }

        //Convert both representation and value type
        template <SPEED_REPRESENTATION OUT, typename V>
            requires std::is_convertible_v<V, T>
        constexpr operator Speed<OUT, V>() const
        {
            const auto converted_speed = convert<OUT>();
            return static_cast<Speed<OUT, V>>(converted_speed);
        }
    };

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
    constexpr Speed<A, T> operator-(const Speed<A, T> &s)
    {
        return Speed<A, T>{-s.value};
    }

    template <typename Op, SPEED_REPRESENTATION A, Number T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<A, T> &rhs, Op op)
    {
        return Speed<A, T>{op(lhs.value, rhs.value)};
    }

    template <typename Op, SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
    constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<B, T> &rhs, Op op)
    {
        const auto rhs_converted = rhs.template convert<A>();
        return Speed<A, T>{op(lhs.value, rhs_converted.value)};
    }

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

    constexpr Speed<SPEED_REPRESENTATION::MS, LiteralBase> operator""_ms(LiteralBase value)
    {
        return Speed<SPEED_REPRESENTATION::MS, LiteralBase>{value};
    }

    constexpr Speed<SPEED_REPRESENTATION::KMH, LiteralBase> operator""_kmh(LiteralBase value)
    {
        return Speed<SPEED_REPRESENTATION::KMH, LiteralBase>{value};
    }
}

template <speed_lib::SPEED_REPRESENTATION r, speed_lib::Number T>
struct std::formatter<speed_lib::Speed<r, T>> : std::formatter<T, char>
{
    const char *format_specifier;
    constexpr auto parse(std::format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end || *it == '}')
        {
            format_specifier = speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::MS>::format_specifier;
            return it;
        }

        auto start = it;
        while (it != end && *it != '}')
            ++it;

        std::string_view spec{&*start, static_cast<size_t>(it - start)};
        if (spec == speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::MS>::suffix)
            format_specifier = speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::MS>::format_specifier;
        else if (spec == speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::KMH>::suffix)
            format_specifier = speed_lib::SpeedLiteralMap<speed_lib::SPEED_REPRESENTATION::KMH>::format_specifier;
        else
            throw std::format_error("invalid format specifier for Speed. Use 'ms' or 'kmh'.");

        return it;
    }

    template <typename FormatContext>
    auto format(const speed_lib::Speed<r, T> &s, FormatContext &ctx) const
    {
        return std::format_to(ctx.out(), "{} {}", s.value, format_specifier);
    }
};