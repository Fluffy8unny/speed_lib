# speed_lib
Simple header only library for handling different representations of speed

## Layout
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
