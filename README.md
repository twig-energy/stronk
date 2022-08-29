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

#### What:
- Easy support for stron{g|k} typing, with plenty of built-in skills to add functionality your own types.
- Automatically combine types with physics-like unit behavior.

#### Why:
- Strong types allow you to catch argument ordering mismatches.
- Unit-like behavior allows you to use the type system to to verify the correctness of your implementations.
- Catch refactoring bugs at compile time by limiting access to the underlying values.

#### How:
```cpp :file=./examples/firstname_lastname_example.cpp
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

// Strong types protects you from accidentally parsing a wrong arguments to wrong positions.
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

```cpp :file=./examples/unit_energy_example.cpp:line_start=0:line_end=23
#include <ratio>

#include <stronk/prefabs.h>
#include <stronk/unit.h>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct Watt : twig::stronk_default_unit<Watt, double>
{
    using stronk_default_unit::stronk_default_unit;
};

void watts_and_identity_units()
{
    Watt watt = Watt {30.};
    watt += Watt {4.} - Watt {2.};  // we can add and subtract units

    // Multiplying and dividing with an identity_unit (such as scalars) does not change the type.
    watt *= 2.;

    // However an identity_unit divided by a regular unit results in a new unit type.
    auto one_over_watt = 1.0 / watt;
    static_assert(!std::is_same_v<decltype(one_over_watt), Watt>);
}
```

Different units can be combined by multiplying or dividing them:

```cpp :file=./examples/unit_energy_example.cpp:line_start=24:line_end=45
// Lets introduce hours as a new unit_like type
struct Hours : twig::stronk<Hours, double, twig::unit>
{
    using stronk::stronk;
};

// We can now dynamically generate a new type!
using WattHours = decltype(Watt {} * Hours {});

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    WattHours watt_hours = Hours {3.5} * Watt {25.0};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours -= WattHours {10.} + WattHours {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    Hours hours = watt_hours / Watt {25.0};
    Watt watt = watt_hours / Hours {3.5};
}
```

These new generated types are also units which can be used to generate new units:

```cpp :file=./examples/unit_energy_example.cpp:line_start=46:line_end=65
// Lets introduce a type for euros, and start combining more types.
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

    // This flexibility allows us to very expessively write code while having the type system checking our code.
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

## Examples:

### Specializers:
In case you want to specialize the resulting type of unit multiplication and division you can utilize the `stronk/specializer.h` header.

By default the units are generated with the `stronk_default_prefab` type.

```cpp :file=./examples/specializers_example.cpp:line_end=22
#include <stronk/specializers.h>

// Lets consider the following units:
struct Distance : twig::stronk<Distance, double, twig::unit>
{
    using stronk::stronk;
};

struct Time : twig::stronk<Time, double, twig::unit>
{
    using stronk::stronk;
};

// Note: For the specializer macros you need to call them from within the twig namespace:
namespace twig
{
// Lets say we want to have Distance / Time specialized to be hashable.
// We can use the STRONK_SPECIALIZE_DIVIDE macro to specialize the generated type.
STRONK_SPECIALIZE_DIVIDE(Distance, Time, can_hash);
// Now any expression resulting the `Distance{} / Time{}` type will result in a unit type with the can_hash skill

}  // namespace twig
```

You can also specialize the underlying type of multiplying two units:
By default the `underlying_type` is the default result of multiplying or dividing the underlying types of the two units themselves.

```cpp :file=./examples/specializers_example.cpp:line_start=23:line_end=29
// Lets specialize Time^2 to use int64_t as its underlying type.
template<>
struct twig::underlying_type_of_multiplying<Time, Time>
{
    using type = int64_t;
};
```

# Using Stronk in Your Project
The project is CMake FetchContent ready and we are working on exposing it on vcpkg.
After retrieving stronk, add the following to your CMakeLists.txt

```cmake
find_package(stronk CONFIG REQUIRED)
target_link_libraries(
    project_target PRIVATE
    twig::stronk
)
```

### Requirements
A c++20 compatible compiler and standard library with concepts support.

We depend on Boost's type_index package to get compile time generated ids for each type to be able to sort types for units (so we can compare types generated from different expressions).

In the extensions subfolder we have added skills for common third party libraries: `fmt`, `absl` and `gtest`. Using any of these will ofcourse also require you to install those dependencies.

# Building and installing

For more information on how to build see the [BUILDING](BUILDING.md) and [HACKING](HACKING.md) documents.

# Licensing

See the [LICENSE](LICENSE.md) document.
