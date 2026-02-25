#include "../include/speed_lib.hpp"
#include <print>
using namespace speed_lib;
int main()
{
    // Literals for speed, that print nicely
    auto speed_str = 10.0_kmh;
    std::println("Parsing speed from '{}'", speed_str);
    // Output: Parsing speed from '10 km/h'

    // fomratting to correct units
    std::println("Parsing speed from '{:ms}'", speed_str);
    // Output: Parsing speed from '2.7777777777777777778 m/s'

    // also allows formating with width and precision
    // {:UNITWIDTH.PRECISIONf} akin to std::format specifiers, but with an additional UNIT specifier
    std::println("Parsing speed from '{:kmh10.2f}'", speed_str);
    // Output: Parsing speed from '     10.00 km/h'

    auto time_str = 30.0_s;
    auto length_str = 100.0_m;
    std::println("Parsing time from '{:min}'", time_str);
    std::println("Parsing length from '{:km}'", length_str);
    // Output: Parsing time from '0.5 min'
    // Output: Parsing length from '0.1 km'

    // Arithmetic operations with automatic unit conversions
    auto speed = length_str / time_str;
    std::println("Calculated speed: {}", speed);
    // Output: Calculated speed: 3.33333 m/s

    // Adding and subtracting same units is implemented, with automatic unit conversions
    auto speed2 = 20.0_kmh;
    auto total_speed = speed + speed2;
    std::println("Total speed: {}", total_speed);
    // Output: Total speed: 25.5556 m/s

    // Scalar multiplication and division is allowed
    auto double_speed = 1.0 * speed * 2.0;
    std::println("Double speed: {}", double_speed);
    // Output: Double speed: 6.66667 m/s

    // Comparing quantities with different units works as well, with automatic unit conversions
    if (10.0_ms < 10.0_kmh)
        std::println("10 m/s is less than 10 km/h...this shouldn't happen");
    else
        std::println("10 m/s is greater than or equal to 10 km/h");
    // Output: 10 m/s is greater than or equal to 10 km/h

    // By not using literals, you can define the type the calculations are performed on
    Time<TIME_REPRESENTATION::S, double> time3{30.0};
    Length<LENGTH_REPRESENTATION::M, double> length3{100.0};
    auto speed4 = length3 / time3;
    std::println("Calculated speed with user-defined types: {}", speed4);
    // Output: Calculated speed with user-defined types: 3.33333 m/s

    Time<TIME_REPRESENTATION::S, int> time5{10};
    Length<LENGTH_REPRESENTATION::M, int> length5{35};
    auto speed5 = length5 / time5;
    std::println("Calculated speed with user-defined types: {}", speed5);
    // Output: Calculated speed with user-defined types: 3 m/s [truncated to int]
    return 0;
}