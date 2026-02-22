#include <concepts>
#include <type_traits>
#include <functional>

enum class SPEED_REPRESENTATION: char{
    MS, //<= meters per second
    KMH //<= kilometers per hour
};

template<typename T>
concept Number = std::is_arithmetic_v<T>;


template<SPEED_REPRESENTATION U, Number T>
struct Speed{
    T value;
    template<SPEED_REPRESENTATION out>
    constexpr Speed<out,T> convert() const {
        if constexpr (U == out){
            return Speed<out,T>{value};
        }
        return Speed<out,T>{value * SpeedMap<U,out,T>::value};
    }

    template<SPEED_REPRESENTATION out>
    constexpr operator Speed<out,T>() const {
        return convert<out>();
    }
};

using DefaultSpeedRepresentation = SPEED_REPRESENTATION::MS;
using SpeedMPS = Speed<SPEED_REPRESENTATION::MS, long double>;


template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator+(const Speed<A, T>& a, const Speed<B, T>& b) {
    return apply_binary_op(a, b, std::plus<T>{});
}

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator-(const Speed<A, T>& a, const Speed<B, T>& b) {
    return apply_binary_op(a, b, std::minus<T>{});
}

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator*(const Speed<A, T>& a, const Speed<B, T>& b) {
    return apply_binary_op(a, b, std::multiplies<T>{});
}

template <SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> operator/(const Speed<A, T>& a, const Speed<B, T>& b) {
    return apply_binary_op(a, b, std::divides<T>{});
}

template <SPEED_REPRESENTATION A, Number T>
constexpr Speed<A, T> operator-(const Speed<A, T>& s) {
    return Speed<A, T>{-s.value};
}

template <typename Op, SPEED_REPRESENTATION A, SPEED_REPRESENTATION B, Number T>
constexpr Speed<A, T> apply_binary_op(const Speed<A, T>& lhs, const Speed<B, T>& rhs, Op op) {
    auto rhs_converted = rhs.template convert<A>();
    return Speed<A, T>{op(lhs.value, rhs_converted.value)};
}

template <SPEED_REPRESENTATION, SPEED_REPRESENTATION, Number>
struct SpeedMap;

template<Number T>
struct SpeedMap<SPEED_REPRESENTATION::MS,SPEED_REPRESENTATION::KMH,T>{
    static constexpr T value{3.6};
};

template<Number T>
struct SpeedMap<SPEED_REPRESENTATION::KMH,SPEED_REPRESENTATION::MS,T>{
    static constexpr T value{1.0/3.6};
};


template<>
struct std::formatter<Speed<SpeedRepresentation r, Number T>> : std::formatter<T, char> {
    std::optional<SPEED_REPRESENTATION> representation;

    constexpr auto parse(std::format_parse_context& ctx) {
            auto it  = ctx.begin();
    auto end = ctx.end();

    if (it == end || *it == '}'){
        representation = SPEED_REPRESENTATION::MS; 
        return it;
    }

    auto start = it;
    while (it != end && *it != '}')
      ++it;
    std::string_view suffix{&*start, static_cast<size_t>(it - start)};
    
    if (spec == "ms") representation = SPEED_REPRESENTATION::MS;
    else if (spec == "kmh") representation = SPEED_REPRESENTATION::KMH;
    else throw std::format_error("invalid format specifier for Speed. Use '_ms' or '_kmh'.");

    return it;
    }

    template <typename FormatContext>
    auto format(const Speed<r, T>& s, FormatContext& ctx) {
        return std::formatter<T, char>::format(s.value, ctx);
    }
};

template<Number T>
constexpr Speed<SPEED_REPRESENTATION::MS, T> operator"" _ms(T value) {
    return Speed<SPEED_REPRESENTATION::MS, T>{value};
}

template<Number T>
constexpr Speed<SPEED_REPRESENTATION::KMH, T> operator"" _kmh(T value) {
    return Speed<SPEED_REPRESENTATION::KMH, T>{value};
}