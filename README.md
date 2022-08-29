[![codecov](https://codecov.io/gh/twig-energy/stronk/branch/main/graph/badge.svg?token=TWO57YT2YA)](https://codecov.io/gh/twig-energy/stronk)
[![license](https://img.shields.io/github/license/twig-energy/stronk)](LICENSE)
```text
    ==================================================================

    *      //   ) ) /__  ___/ //   ) )  //   ) ) /|    / / //   / /  *
    *     ((          / /    //___/ /  //   / / //|   / / //__ / /   *
    *       \\       / /    / ___ (   //   / / // |  / / //__  /     *
    *         ) )   / /    //   | |  //   / / //  | / / //   \ \     *
    *  ((___ / /   / /    //    | | ((___/ / //   |/ / //     \ \    *

    ==================================================================
```

=== An easy to customize, strong type library with built in support for unit-like behavior ===

```cpp:file=./examples/firstname_lastname_example.cpp
#include <iostream>
#include <string>

#include <stronk/stronk.h>

struct FirstName : twig::stronk<FirstName, std::string, twig::can_stream>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string, twig::can_stream>
{
    using stronk::stronk;
};

// Strong types protects you from accidentially parsing a wrong arguments to wrong positions.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    std::cout << firstname << " " << lastname << std::endl;
}

auto main() -> int
{
    print_name(LastName {"Doe"}, FirstName {"John"});
}
```

On top of providing strong type utilities, `stronk` also enables unit like behavior:

```cpp:file=./examples/unit_energy_example.cpp:line_start=0:line_end=29
#include <ratio>

#include <stronk/prefabs.h>
#include <stronk/unit.h>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct Watt : twig::stronk_default_unit<Watt, double>
{
    using stronk_default_unit::stronk_default_unit;
};
// And we also try to add an identity-unit type
struct MyScalar : twig::stronk<MyScalar, double, twig::identity_unit>
{
    using stronk::stronk;
};

void watts_and_identity_units()
{
    Watt watt = Watt {30.};
    watt += Watt {4.} - Watt {2.};  // we can add and subtract units

    // Multiplying and dividing with identity unit does not change the type.
    watt *= 2.;
    watt /= MyScalar {2.};

    // However as identity unit divided by a unit results in a new unit type of.
    auto one_over_watt = 1.0 / watt + MyScalar {1.0} / watt;  // NOLINT
    static_assert(!std::is_same_v<decltype(one_over_watt), Watt>);
}
```

These new generated types are also units which can be used to generate new units:

```cpp:file=./examples/unit_energy_example.cpp:line_start=31:line_end=53
// Next we introduce hours and a new unit_like type
struct Hours : twig::stronk<Hours, double, twig::unit>
{
    using stronk::stronk;
};

// We can now dynamically generate a new compile time type based on our stronk types.
// The generated type is very verbose so adding an alias can be helpful
using WattHours = decltype(Watt {} * Hours {});

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    WattHours watt_hours = Hours {3.5} * Watt {25.0};

    // this new type can also add and subtract from itself.
    watt_hours -= WattHours {10.} + WattHours {2.};

    // We can get back the hours or the watt by dividing by them out.
    Hours hours = watt_hours / Watt {25.0};
    Watt watt = watt_hours / Hours {3.5};
}
```

```cpp:file=./examples/unit_energy_example.cpp:line_start=55:line_end=74
// Lets introduce money so we can really start combining types.
struct Euro : twig::stronk<Euro, double, twig::unit>
{
    using stronk::stronk;
};

void introducing_another_type()
{
    // There are make functions for std::ratio like types. It simply scales the input value and does not change the type
    WattHours watt_hours = twig::make<std::mega, WattHours>(1.);
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_watt_hour = Euro {300.} / watt_hours;

    // which ofcours also can be handled like any other unit which allows this flexible strongly typed system.
    Euro price_for_buying_5_megawatt = euros_per_watt_hour * twig::make<std::mega, WattHours>(5.);

    auto watt_hours_per_euro = 1. / euros_per_watt_hour;  // `(Watt * Hours) / Euro`
    WattHours watt_hours_affordable_for_500_euros = watt_hours_per_euro * Euro {500.};
}
```

Units are a great way of using the type system to validate your code.

## Current list of skills
Skills adds functionality to your stronk types. We have implemented a number of generic skills which should help you get started.

### Regular
- `can_negate`: unary `operator-`
- `can_add`: binary `operator+` `operator=+`
- `can_subtract`: binary `operator-` and `operator=-`
- `can_multiply`: binary `operator*` and `operator=*` (not compatible with units, we encourage you to use units instead)
- `can_divide`: binary `operator/` and `operator=/` (not compatible with units, we encourage you to use units instead)
- `can_abs`: `twig::abs` will accept strong-types with this skill. `twig::abs` will not accept strong types without this skill.
- `can_isnan`: `twig::isnan` will accept strong-types with this skill
- `can_stream`: `operator<<(std::ostream)`, stream the underlying value to the stream.
- `can_order`: `operator<=>`, note you probably also want to add `can_equate` or one of its relatives, since the compiler cannot generate equality with the `operator<=>` for stronk types (since its not default implemented)
- `can_equate`: `operator==` with regular equality
- `can_equate_with_is_close`: `operator==` but with numpy's `is_close` definition of equal
- `can_equate_with_is_close_nan_equals`: `operator==` but with numpy's `is_close` definition of equal, nans being equal
- `can_equate_with_is_close_abs_tol_only`: `operator==` with a small absolute tolerance for difference
- `can_less_than_greater_than`: `operator<` and `operator>` (prefer the `can_order` skill instead)
- `can_less_than_greater_than_or_equal`: operator <= and operator >= (prefer the `can_order` skill instead)
- `can_be_used_as_flag`: for boolean values used as flags
- `can_hash`: implements the `std::hash<T>`.

### Units
- `unit`: enables unit behavior for multiplication and division.
- `identity_unit`: enables unit behavior, but does not affect the type of multiplication and division.

### Third Party Library extensions (see `stronk/extensions/<library>.h`):
- `can_absl_hash`: implements the `AbslHashValue` friend function.
- `can_gtest_print`: for printing the values in gtest check macros
- `can_fmt_format`: implements `struct fmt::formatter<T>` with default formatting string `"{}"`. In the future we will add a `can_format` for `std::format`.
- `can_fmt_format_builder<"fmt format string{}">::skill`: implements `struct fmt::formatter<T>`. In the future we will add a `can_format_builder<"std format string">` for `std::format`.
### Prefabs: (see `stronk/prefabs.h`)
- `stronk_default_unit`: arithmetic unit with most of the regular operations
- `stronk_flag`: flag like boolean with equal operators etc.

Adding new skills is easy so feel free to add more.


Credit to https://www.foonathan.net/2016/10/strong-typedefs/ for a great amount of inspiration

# Using Stronk in Your Project
The project is CMake FetchContent ready and we are working on exposing it on vcpkg.
After retreiving stronk, add the following to your CMakeLists.txt

```cmake
find_package(stronk CONFIG REQUIRED)
target_link_libraries(
    project_target PRIVATE
    twig::stronk
)
```

### Dependencies
We depend on Boost's type_index package to get compile time generated ids for each type to be able to sort types for units (so we can compare types generated from different expressions).

In the extensions subfolder we have added skills for common third party libraries: `fmt`, `absl` and `gtest`. Using any of these will ofcourse also require you to install those dependencies.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

See the [LICENSE](LICENSE.md) document.
