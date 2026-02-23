#include <concepts>
#include <type_traits>
#include <format>
#include <functional>

enum class SPEED_REPRESENTATION : char
{
    MS, //<= meters per second
    KMH //<= kilometers per hour
};

template <typename T>
concept Number = std::is_arithmetic_v<T>;

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
};

template<SPEED_REPRESENTATION A>
struct SpeedLiteralMap;

template<>
struct SpeedLiteralMap<SPEED_REPRESENTATION::MS>
{
    static constexpr const char* suffix = "ms";
    static constexpr const char* format_specifier = "m/s";
};

template<>
struct SpeedLiteralMap<SPEED_REPRESENTATION::KMH>
{
    static constexpr const char* suffix = "kmh";
    static constexpr const char* format_specifier = "km/h";
};

constexpr SPEED_REPRESENTATION DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
using LiteralBase = long double;
using SpeedMPS = Speed<SPEED_REPRESENTATION::MS, LiteralBase>;

template <SPEED_REPRESENTATION A,  Number T>
constexpr Speed<A, T> operator+(const Speed<A, T> &a, const Speed<A, T> &b)
{
    return Speed<A, T>{a.value + b.value};
}

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator+(const Speed<A, T> &a, const Speed<B, T> &b)
{
    return apply_binary_op(a, b, std::plus<T>{});
}

template <SPEED_REPRESENTATION A,  Number T>
constexpr Speed<A, T> operator-(const Speed<A, T> &a, const Speed<A, T> &b)
{
    return Speed<A, T>{a.value - b.value};
}

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator-(const Speed<A, T> &a, const Speed<B, T> &b)
{
    return apply_binary_op(a, b, std::minus<T>{});
}

template <SPEED_REPRESENTATION A,  Number T>
constexpr Speed<A, T> operator*(const Speed<A, T> &a, const Speed<A, T> &b)
{   
    return Speed<A, T>{a.value * b.value};
}   

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator*(const Speed<A, T> &a, const Speed<B, T> &b)
{
    return apply_binary_op(a, b, std::multiplies<T>{});
}

template <SPEED_REPRESENTATION A,  Number T>
constexpr Speed<A, T> operator/(const Speed<A, T> &a, const Speed<A, T> &b)
{
    return Speed<A, T>{a.value / b.value};
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

template <typename Op, SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> apply_binary_op(const Speed<A, T> &lhs, const Speed<B, T> &rhs, Op op)
{
    auto rhs_converted = rhs.template convert<A>();
    return Speed<A, T>{op(lhs.value, rhs_converted.value)};
}

template <SPEED_REPRESENTATION r, Number T>
struct std::formatter<Speed<r, T>> : std::formatter<T, char>
{
    const char* format_specifier;
    constexpr auto parse(std::format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end || *it == '}')
        {
            format_specifier = SpeedLiteralMap<SPEED_REPRESENTATION::MS>::format_specifier;
            return it;
        }

        auto start = it;
        while (it != end && *it != '}')
            ++it;

        std::string_view spec{&*start, static_cast<size_t>(it - start)};
        if (spec == SpeedLiteralMap<SPEED_REPRESENTATION::MS>::suffix)
            format_specifier = SpeedLiteralMap<SPEED_REPRESENTATION::MS>::format_specifier;
        else if (spec == SpeedLiteralMap<SPEED_REPRESENTATION::KMH>::suffix)
            format_specifier = SpeedLiteralMap<SPEED_REPRESENTATION::KMH>::format_specifier;
        else
            throw std::format_error("invalid format specifier for Speed. Use 'ms' or 'kmh'.");

        return it;
    }

    template <typename FormatContext>
    auto format(const Speed<r, T> &s, FormatContext &ctx) const
    {
        std::string_view fmt = std::format("{} {}", s.value, format_specifier);
        return std::format_to(ctx.out(), "{}", fmt);
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