# speed_lib
Header-only, compile-time units library with custom literals, automatic cross-unit conversion, and compile-time errors for invalid type/dimension operations, so these safety checks add no runtime slowdown.

## Motivation

The motivation for this library comes from professional projects where a lot of time was lost to bugs caused by unit mismatches (for example, m/s vs km/h) between teammates and system boundaries. `speed_lib` exists to prevent those bugs in the future by making invalid unit usage fail at compile time instead of slipping into runtime behavior.


## Features

This section mirrors [examples/example.cpp](examples/example.cpp).

```cpp
#include "speed_lib.hpp"
#include <print>

using namespace speed_lib;

int main()
{
	auto speed = 10.0_kmh;
	std::println("Default: {}", speed);            // 10 km/h
	std::println("As m/s: {:ms}", speed);          // 2.777... m/s
	std::println("Formatted: {:kmh10.2f}", speed); //      10.00 km/h
}
```

### Derived quantities and arithmetic

```cpp
auto t = 30.0_s;
auto d = 100.0_m;

auto v = d / t;               // Length / Time -> Speed
auto total = v + 20.0_kmh;    // automatic unit conversion
auto scaled = 2.0 * v;
```

### Comparisons

```cpp
if (10.0_ms >= 10.0_kmh)
{
	// true
}
```

### Explicit numeric types

```cpp
Time<TIME_UNIT::S, int> t{30};
Length<LENGTH_UNIT::M, int> d{100};
auto v = d / t; // Speed<SPEED_UNIT::MS, int>
```

### Compile-time type validation

```cpp
auto d = 100.0_m;
auto t = 30.0_s;

// auto invalid = d + t; // compile-time error: incompatible dimensions (Length + Time)
```

### Formatting syntax

The quantity formatter accepts:

- `UNIT` (required), e.g. `ms`, `kmh`, `min`, `km`
- optional `WIDTH`, e.g. `{:kmh10}`
- optional `.PRECISIONf`, e.g. `{:kmh10.2f}`

Pattern: `{:UNITWIDTH.PRECISIONf}`

## Installation

`speed_lib` is header-only. There is nothing to link and no external dependency to install; it only depends on the C++ standard library.
Copy or vendor the `include/` directory into your project and add it to your compiler include paths.
You need a C++23-capable compiler and standard library.

## Extending with a new unit dimension

1. Add a new header under `include/speed_lib/units/` (for example `mass.hpp`).
2. Define your enum (base unit first), then invoke the macros:
	- `SPEED_LIB_DEFINE_TAG_WITH_UNIT(...)`
	- `SPEED_LIB_DEFINE_UNIT_TRAITS(...)` for each unit
	- `SPEED_LIB_DEFINE_LITERAL_OPERATOR(...)` for literals
3. Add your formatter specialization 
4. Add your alias template (like `using Mass = Quantity<...>`).
5. Include your new unit header in [include/speed_lib.hpp](include/speed_lib.hpp).
6. If needed, add cross-dimension operators (e.g. length/time -> speed) in [include/speed_lib/mixed_operations.hpp](include/speed_lib/mixed_operations.hpp).

### Example implementation
Example `include/speed_lib/units/mass.hpp`:

```cpp
#pragma once

#include "../common.hpp"
#include "../formatting.hpp"
#include "../detail/unit_macros.hpp"

namespace speed_lib
{
	enum class MASS_UNIT : char
	{
		G,  // base unit
		KG,
		LB
	};

	SPEED_LIB_DEFINE_TAG_WITH_UNIT(MassTag, MASS_UNIT)

	SPEED_LIB_DEFINE_UNIT_TRAITS(MassTag, MASS_UNIT::G, 1.0L, 0.0L, "g", "g", "Weight")
	SPEED_LIB_DEFINE_UNIT_TRAITS(MassTag, MASS_UNIT::KG, 1000.0L, 0.0L, "kg", "kg", "Weight")
	SPEED_LIB_DEFINE_UNIT_TRAITS(MassTag, MASS_UNIT::LB, 453.59237L, 0.0L, "lb", "lb", "Weight")

	template <MASS_UNIT Unit, typename ValueType>
	using Mass = Quantity<MassTag, Unit, ValueType>;

	SPEED_LIB_DEFINE_LITERAL_OPERATOR(MassTag, MASS_UNIT, G, g);
	SPEED_LIB_DEFINE_LITERAL_OPERATOR(MassTag, MASS_UNIT, KG, kg);
	SPEED_LIB_DEFINE_LITERAL_OPERATOR(MassTag, MASS_UNIT, LB, lb);
}

template <speed_lib::MASS_UNIT Unit, speed_lib::NumericalType ValueType>
struct std::formatter<speed_lib::Mass<Unit, ValueType>>
	: speed_lib::QuantityFormatter<
		  speed_lib::MassTag,
		  Unit,
		  ValueType,
		  speed_lib::MASS_UNIT::G,
		  speed_lib::MASS_UNIT::KG,
		  speed_lib::MASS_UNIT::LB>
{
};

#include "../detail/unit_macros_undef.hpp"
```

Then add `#include "speed_lib/units/mass.hpp"` to [include/speed_lib.hpp](include/speed_lib.hpp).
