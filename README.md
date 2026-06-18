[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=twig-energy_stronk&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=twig-energy_stronk)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=twig-energy_stronk&metric=coverage)](https://sonarcloud.io/summary/new_code?id=twig-energy_stronk)
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

## An easy to customize, strong type library with built-in support for unit-like behavior

#### What

- Easy support for stron{g|k} typing, with plenty of built-in skills to add functionality to your own types.
- Automatically combine types with physics-like unit behavior: `10 [Meter] / 2 [Second] = 5 [Meter / Second]`.

#### Why

- Strong types allow you to catch argument ordering mismatches.
- Unit-like behavior allows you to use the type system to verify the correctness of your implementation.
- Catch refactoring bugs at compile time by limiting access to the underlying values.

#### How

```cpp :file=./examples/firstname_lastname_example.cpp
#include <iostream>
#include <string>

#include <stronk/skills/can_stream.hpp>
#include <stronk/stronk.hpp>

struct FirstName : twig::stronk<FirstName, std::string>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string, twig::can_ostream>
{
    using stronk::stronk;
};

// Strong types protects you from accidentally passing the wrong argument to the wrong parameter.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    // You can access the underlying type by using the .unwrap<Type>() function.
    std::cout << firstname.unwrap<FirstName>() << " ";
    // Adding the twig::can_ostream skill overloads the `operator<<(ostream&)` for your type.
    std::cout << lastname << std::endl;
    // Generally you should prefer skills or custom functions over unwrapping to preserve your strong types
}

auto main() -> int
{
    print_name(LastName {"Doe"}, FirstName {"John"});
}
```

On top of providing strong type utilities, `stronk` also enables unit-like behavior:

```cpp :file=./examples/unit_energy_example.cpp:line_start=0:line_end=27
#include <concepts>

#include <stronk/stronk.hpp>
#include <stronk/unit.hpp>

#include "stronk/utilities/ratio.hpp"

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct joules : twig::stronk_default_unit<joules, twig::ratio<1>>
{
};

template<typename T>
using joules_t = joules::value<T>;

void joules_and_identity_units()
{
    auto energy = joules_t {30.};
    energy += joules_t {4.} - joules_t {2.};  // we can add and subtract units

    // Multiplying and dividing with an identity_unit (such as floats and integers) does not change the type.
    energy *= 2.;

    // However an identity_unit divided by a regular unit results in a new unit type.
    auto one_over_joules = 1.0 / energy;
    static_assert(!std::same_as<decltype(one_over_joules), joules_t<double>>);
}
```

Different units can be combined by multiplying or dividing them:

```cpp :file=./examples/unit_energy_example.cpp:line_start=28:line_end=61
// Let's introduce seconds as a new unit
struct seconds : twig::stronk_default_unit<seconds, twig::ratio<1>>
{
};

// We can define ratios of a specific unit - these scaled units have the same dimension
template<typename T>
using hours_t = seconds::scaled_t<twig::ratio<60 * 60>>::value<T>;

// We can now dynamically generate a new type!
using watt = twig::divided_unit_t<joules, seconds>;

template<typename T>
using watt_t = watt::value<T>;

// or make custom names for already known types (joules) with specific scale
using watt_hours = decltype(watt_t<double> {} * hours_t<double> {})::unit_t;

template<typename T>
using watt_hours_t = watt_hours::value<T>;

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    watt_hours_t watt_hours_val = hours_t {3.} * watt_t {25.};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours_val -= watt_hours_t {10.} + watt_hours_t {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    hours_t hours_val = watt_hours_val / watt_t {25.};
    watt_t watt_val = watt_hours_val / hours_t {3.};
}
```

These new generated types are also units which can be used to generate new units:

```cpp :file=./examples/unit_energy_example.cpp:line_start=62:line_end=86
// Let's introduce a type for euros, and start combining more types.
struct euro : twig::stronk_default_unit<euro, twig::ratio<1>>
{
};
template<typename T>
using euro_t = euro::value<T>;

template<typename T>
using mega_watt_hours_t = joules::scaled_t<twig::ratio_multiply<twig::mega, typename watt_hours::scale_t>>::value<T>;

void introducing_another_type()
{
    // twig::make allows you to scale the input value but it does not change the resulting type
    auto one_mega_watt_hour = mega_watt_hours_t {1.};
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_mega_watt_hour = euro_t {300.} / one_mega_watt_hour;

    // This flexibility allows us to write expessive code, while having the type system check our implementation.
    euro_t price_for_buying_5_mega_watt_hours =
        euros_per_mega_watt_hour * (twig::identity_value_t<twig::mega, double> {1} * watt_hours_t {5.});

    auto mega_watt_hours_per_euro = 1. / euros_per_mega_watt_hour;  // `(Watt * Hours) / Euro`
    mega_watt_hours_t mega_watt_hours_affordable_for_500_euros = mega_watt_hours_per_euro * euro_t {500.};
}
```

Units are a great way of using the type system to validate your code.

Credit to [Jonathan Müller](https://github.com/foonathan)'s [blogpost](https://www.foonathan.net/2016/10/strong-typedefs/) and [Jonathan Boccara](https://github.com/joboccara)'s [blogpost](https://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/) - both of which have been great sources of inspiration.

## Current list of skills

Skills adds functionality to your stronk types. We have implemented a number of generic skills which should help you get started.

### Regular

- `can_negate`: unary `operator-`
- `can_add`: binary `operator+` `operator+=`
- `can_subtract`: binary `operator-` and `operator-=`
- `can_multiply`: binary `operator*` and `operator*=` (not compatible with units, we encourage you to use units instead)
- `can_multiply_with<T>`: binary `operator*` and `operator*=` with a raw type `T` (not compatible with units)
- `can_divide`: binary `operator/` and `operator/=` (not compatible with units, we encourage you to use units instead)
- `can_divide_with<T>`: binary `operator/` and `operator/=` by a raw type `T` (not compatible with units)
- `can_isnan`: `.isnan()` member, `.quiet_NaN()` and `.signaling_NaN()` static members for floating point types
- `can_stream`: overloads `operator<<(std::ostream)` and `operator<<(std::istream)`, stream the underlying value to the stream, or create from stream. For only `ostream` or `istream` functionality, use `can_ostream` or `can_istream` respectively.
- `can_order`: `operator<=>`, note you probably also want to add `can_equate`, since the compiler cannot generate equality with the `operator<=>` for stronk types.
- `can_equate`: `operator==` with regular equality
- `can_equate_with_is_close`: `operator==` but with numpy's `is_close` definition of equal
- `can_equate_with_is_close_nan_equals`: `operator==` but with numpy's `is_close` definition of equal, nans being equal
- `can_equate_with_is_close_abs_tol_only`: `operator==` with a small absolute tolerance for difference
- `can_less_than_greater_than`: `operator<` and `operator>` (prefer the `can_order` skill instead)
- `can_less_than_greater_than_or_equal`: `operator<=` and `operator>=` (prefer the `can_order` skill instead)
- `can_be_used_as_flag`: for boolean values used as flags
- `can_hash`: specializes `std::hash<T>`.
- `can_size`: implements `.size()` and `.empty()`
- `can_const_iterate`: implements `begin() const`, `end() const`, `cbegin() const` and `cend() const`.
- `can_iterate`: adds the `can_const_iterate` as well implementing `begin()`, `end()`.
- `can_const_index`: implements `operator[](const auto&) const` and `at(const auto&) const`
- `can_index`: adds the `can_const_index` as well implementing `operator[](const auto&)` and `at(const auto&)`.
- `can_increment`: adds both `operator++` operators.
- `can_decrement`: adds both `operator--` operators.
- `can_be_const_viewed_as<ViewT>`: adds an implicit conversion to a const `view_t` wrapping `ViewT`, such as `std::span<const T>` or `std::string_view`.
- `can_be_mutable_viewed_as<ViewT>`: adds an implicit conversion to a `mutable_view_t` wrapping `ViewT`, such as `std::span<T>`.

### Math free functions (see `stronk/cmath.hpp`)

Including `stronk/cmath.hpp` provides `twig::` free functions that work for any stronk-like type without requiring a skill:

- `twig::abs(x)`: absolute value
- `twig::isnan(x)`: checks for NaN
- `twig::log(x)`, `twig::log2(x)`, `twig::log10(x)`: logarithms (non-unit types only)
- `twig::sqrt(x)`: square root; for unit types this also infers the correct resulting unit
- `twig::pow<N>(x)`: integer power; for unit types this also infers the correct resulting unit

### Third Party Library extensions (see `stronk/extensions/<library>.hpp`)

Some extensions just require including the header; others require adding a skill to your type.

**Header-only (no skill needed):**
- `doctest.hpp` — specializes `doctest::StringMaker<T>` so stronk values print in doctest assertions.
- `fmt.hpp` — specializes `fmt::formatter<T>` for all stronk types using the underlying type's formatter by default. Optionally add the `can_fmt_format` skill for a `"{}"` format string, or `can_fmt_format_builder<"my format {}">::skill` for a custom format string.
- `glaze.hpp` — specializes `glz::meta<T>` for Glaze JSON serialization.
- `gtest.hpp` — defines a `PrintTo` free function for any stronk type whose underlying type supports `operator<<`, enabling printing in gtest assertions.
- `nlohmann_json.hpp` — specializes `nlohmann::adl_serializer<T>` for nlohmann JSON serialization/deserialization.
- `stronk/skills/can_format.hpp` — specializes `std::formatter<T>` so all stronk types work with `std::format` out of the box.

**Requires adding a skill to your type:**
- `absl.hpp` — `can_absl_hash` skill: implements the `AbslHashValue` friend function.

Adding new skills is easy so feel free to add more.

## Prefabs: (see `stronk/prefabs/<prefab>.hpp`)

Often you might just need a group of skills for your specific types. For this you can use prefabs.

- `stronk_arithmetic`: a stronk number with addition, subtraction, negation, equation and ordering skills.
- `stronk_flag`: a stronk flag-like boolean with equal operators etc.
- `stronk_string`: a stronk string with equation and size skills.
- `stronk_vector`: a stronk std::vector with equation, indexing, iterating and size skills.

## Examples

### Specializers

Specialization of unit multiplication and division is possible

By default the units are generated with the `stronk_default_unit` type.

```cpp :file=./examples/specializers_example.cpp:line_end=33
#include <concepts>
#include <cstdint>

#include "stronk/unit.hpp"
#include "stronk/utilities/ratio.hpp"

// Let's consider the following units:
struct meters_unit : twig::unit<meters_unit, twig::ratio<1>>
{
};

struct seconds : twig::unit<seconds, twig::ratio<1>>
{
};

// Let's say you want to use a custom defined stronk type for certain unit combinations.
// Let's introduce our own `Speed` type:
struct meters_per_second_unit : twig::unit<twig::divided_dimensions_t<meters_unit, seconds>, twig::ratio<1>>
{
};
// Notice we are using twig::divided_dimensions_t instead of the regular tag

// To make it possible for stronk to find this type we need to specialize `unit_lookup`:
template<>
struct twig::unit_lookup<twig::divided_dimensions_t<meters_unit, seconds>>
{
    template<scale_like ScaleT>  // scale is to support kilo meters / second, or nano meters / second
    using unit_t = twig::unit_scaled_or_base_t<meters_per_second_unit, ScaleT>;
};

// Now the automatically generated stronk unit for seconds^2 is meters_per_second

// The above of course also works for `multiplied_unit` and `unit_multiplied_resulting_unit_type`
```

# Using Stronk in Your Project

The project is CMake FetchContent ready and is available on [vcpkg](https://github.com/microsoft/vcpkg/tree/master/ports/stronk).
After retrieving stronk, add the following to your CMakeLists.txt

```cmake
find_package(stronk CONFIG REQUIRED)
target_link_libraries(
    project_target PRIVATE
    twig::stronk
)
```

# Requirements

A c++20 compatible compiler and standard library with concepts support.

We depend on Boost's type_index package to get compile time generated ids for each type to be able to sort types for units (so we can compare types generated from different expressions).

In the extensions subfolder we have added skills for common third party libraries: `fmt`, `absl` and `gtest`. Using these also requires the relevant third party libraries to be installed.

# Building and installing

For more information on how to build see the [BUILDING](BUILDING.md) and [HACKING](HACKING.md) documents.

# Benchmarks

Stronk is a close to zero cost abstraction - performance varies per compiler. Clang-22 especially achieves full parity across the board.
You can see benchmark results for all the tested platforms in the [Continuous Integration Workflow](https://github.com/twig-energy/stronk/actions/workflows/ci.yml).

| relative |               ns/op |                op/s |    err% |     total | Default Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------------------------
|   100.0% |                0.58 |    1,729,283,826.39 |    0.4% |      0.01 | `int8_t`
| 4,993.0% |                0.01 |   86,343,991,349.56 |    0.2% |      0.01 | `stronk_int8_t`
|   100.0% |                0.09 |   10,907,066,439.60 |    0.2% |      0.01 | `int64_t`
|   100.1% |                0.09 |   10,921,367,074.58 |    0.2% |      0.01 | `stronk_int64_t`
|   100.0% |                2.29 |      436,287,719.82 |    0.2% |      0.01 | `std::string`
|    93.7% |                2.45 |      408,625,531.99 |    0.1% |      0.01 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Random Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------------------------
|   100.0% |                5.15 |      194,287,089.60 |    0.2% |      0.01 | `int8_t`
|    99.4% |                5.18 |      193,199,485.51 |    0.3% |      0.01 | `stronk_int8_t`
|   100.0% |               17.79 |       56,218,229.95 |    0.1% |      0.01 | `int64_t`
|    99.9% |               17.80 |       56,180,171.57 |    0.1% |      0.01 | `stronk_int64_t`
|   100.0% |              340.96 |        2,932,889.39 |    0.1% |      0.03 | `std::string`
|    99.9% |              341.38 |        2,929,307.92 |    0.1% |      0.03 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Copy Vector Benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------
|   100.0% |              218.45 |        4,577,785.23 |    0.1% |      0.01 | `int8_t`
|   121.4% |              179.90 |        5,558,711.08 |    0.2% |      0.01 | `stronk_int8_t`
|   100.0% |            1,506.59 |          663,750.72 |    0.2% |      0.01 | `int64_t`
|   103.5% |            1,456.11 |          686,760.03 |    0.1% |      0.01 | `stronk_int64_t`
|   100.0% |           49,459.91 |           20,218.39 |    0.1% |      0.01 | `std::string`
|    99.7% |           49,584.65 |           20,167.53 |    0.1% |      0.01 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Add Units
|---------:|--------------------:|--------------------:|--------:|----------:|:----------
|   100.0% |                1.17 |      858,316,784.76 |    0.1% |      0.01 | `int8_t + int8_t`
|    99.5% |                1.17 |      853,945,194.38 |    0.4% |      0.01 | `stronk_int8_t + stronk_int8_t`
|   100.0% |                1.28 |      782,519,917.31 |    0.4% |      0.01 | `int64_t + int64_t`
|   100.1% |                1.28 |      783,287,086.36 |    0.2% |      0.01 | `stronk_int64_t + stronk_int64_t`
|   100.0% |                1.18 |      846,734,404.56 |    0.9% |      0.01 | `double + double`
|   100.1% |                1.18 |      847,202,451.77 |    0.9% |      0.01 | `stronk_double_t + stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Add Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |                0.59 |    1,696,577,433.32 |    0.2% |      0.01 | `int8_t + int8_t`
|   108.8% |                0.54 |    1,846,548,026.85 |    0.2% |      0.01 | `stronk_int8_t + stronk_int8_t`
|   100.0% |                0.66 |    1,525,615,413.27 |    0.2% |      0.01 | `int64_t + int64_t`
|    99.9% |                0.66 |    1,524,443,374.13 |    0.3% |      0.01 | `stronk_int64_t + stronk_int64_t`
|   100.0% |                0.63 |    1,590,073,078.43 |    0.1% |      0.01 | `double + double`
|   100.1% |                0.63 |    1,591,981,547.05 |    0.1% |      0.01 | `stronk_double_t + stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |                1.17 |      857,136,588.93 |    0.1% |      0.01 | `int8_t - int8_t`
|    96.7% |                1.21 |      829,193,233.18 |    0.5% |      0.01 | `stronk_int8_t - stronk_int8_t`
|   100.0% |                1.29 |      778,149,406.28 |    0.5% |      0.01 | `int64_t - int64_t`
|   100.2% |                1.28 |      779,373,232.54 |    0.4% |      0.01 | `stronk_int64_t - stronk_int64_t`
|   100.0% |                1.19 |      842,407,258.10 |    1.1% |      0.01 | `double - double`
|    98.9% |                1.20 |      833,028,269.27 |    1.6% |      0.01 | `stronk_double_t - stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------
|   100.0% |                0.59 |    1,698,959,131.59 |    0.3% |      0.01 | `int8_t - int8_t`
|   108.7% |                0.54 |    1,846,393,421.35 |    0.3% |      0.01 | `stronk_int8_t - stronk_int8_t`
|   100.0% |                0.65 |    1,533,862,322.47 |    0.1% |      0.01 | `int64_t - int64_t`
|    99.9% |                0.65 |    1,532,172,012.84 |    0.1% |      0.01 | `stronk_int64_t - stronk_int64_t`
|   100.0% |                0.63 |    1,589,624,481.47 |    0.1% |      0.01 | `double - double`
|   100.0% |                0.63 |    1,590,338,610.50 |    0.2% |      0.01 | `stronk_double_t - stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | multiply_units_benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:--------------------------
|   100.0% |                1.22 |      822,325,694.23 |    0.3% |      0.01 | `int8_t * int8_t`
|    99.8% |                1.22 |      820,663,516.60 |    0.2% |      0.01 | `stronk_int8_t * stronk_int8_t`
|   100.0% |                1.23 |      812,871,722.34 |    0.5% |      0.01 | `int64_t * int64_t`
|    99.0% |                1.24 |      805,050,105.18 |    0.5% |      0.01 | `stronk_int64_t * stronk_int64_t`
|   100.0% |                1.20 |      836,237,192.47 |    1.3% |      0.01 | `double * double`
|   100.8% |                1.19 |      843,253,066.79 |    1.3% |      0.01 | `stronk_double_t * stronk_double_t`
|   100.0% |                1.21 |      828,675,868.33 |    0.2% |      0.01 | `int64_t * double`
|    99.9% |                1.21 |      827,710,995.26 |    0.3% |      0.01 | `stronk_int64_t * stronk_double_t`
|   100.0% |                1.20 |      835,505,080.58 |    0.4% |      0.01 | `double * int64_t`
|    99.9% |                1.20 |      835,070,700.19 |    0.4% |      0.01 | `stronk_double_t * stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Multiply Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------
|   100.0% |                0.64 |    1,571,860,765.25 |    7.1% |      0.01 | :wavy_dash: `int8_t * int8_t` (Unstable with ~199.8 iters. Increase `minEpochIterations` to e.g. 1998)
|   106.9% |                0.60 |    1,679,897,693.89 |    0.3% |      0.01 | `stronk_int8_t * stronk_int8_t`
|   100.0% |                0.64 |    1,552,427,134.81 |    0.1% |      0.01 | `int64_t * int64_t`
|   100.0% |                0.64 |    1,552,440,152.31 |    0.2% |      0.01 | `stronk_int64_t * stronk_int64_t`
|   100.0% |                0.63 |    1,576,030,781.85 |    0.2% |      0.01 | `double * double`
|   100.1% |                0.63 |    1,578,001,423.77 |    0.1% |      0.01 | `stronk_double_t * stronk_double_t`
|   100.0% |                0.63 |    1,585,578,052.75 |    0.1% |      0.01 | `int64_t * double`
|   100.0% |                0.63 |    1,585,654,130.16 |    0.1% |      0.01 | `stronk_int64_t * stronk_double_t`
|   100.0% |                0.63 |    1,590,924,110.18 |    0.1% |      0.01 | `double * int64_t`
|    99.9% |                0.63 |    1,589,422,415.74 |    0.1% |      0.01 | `stronk_double_t * stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Divide Units
|---------:|--------------------:|--------------------:|--------:|----------:|:-------------
|   100.0% |                1.48 |      676,264,337.89 |    0.4% |      0.01 | `int8_t / int8_t`
|    99.8% |                1.48 |      674,892,282.51 |    0.3% |      0.01 | `stronk_int8_t / stronk_int8_t`
|   100.0% |                2.46 |      406,103,297.36 |    0.1% |      0.01 | `int64_t / int64_t`
|   100.0% |                2.46 |      405,980,756.74 |    0.1% |      0.01 | `stronk_int64_t / stronk_int64_t`
|   100.0% |                1.66 |      601,854,501.30 |    0.7% |      0.01 | `double / double`
|    99.6% |                1.67 |      599,536,296.15 |    0.6% |      0.01 | `stronk_double_t / stronk_double_t`
|   100.0% |                1.68 |      594,760,764.77 |    0.5% |      0.01 | `int64_t / double`
|   100.5% |                1.67 |      597,440,457.73 |    0.5% |      0.01 | `stronk_int64_t / stronk_double_t`
|   100.0% |                1.65 |      605,562,240.69 |    0.4% |      0.01 | `double / int64_t`
|    99.7% |                1.66 |      603,881,819.78 |    0.3% |      0.01 | `stronk_double_t / stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Divide Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:----------------------
|   100.0% |                2.72 |      367,568,537.71 |    0.1% |      0.01 | `int8_t / int8_t`
|   100.0% |                2.72 |      367,544,716.74 |    0.1% |      0.01 | `stronk_int8_t / stronk_int8_t`
|   100.0% |                2.46 |      406,420,540.85 |    0.1% |      0.01 | `int64_t / int64_t`
|   100.0% |                2.46 |      406,312,700.27 |    0.1% |      0.01 | `stronk_int64_t / stronk_int64_t`
|   100.0% |                1.76 |      567,586,691.30 |    0.1% |      0.01 | `double / double`
|    99.9% |                1.76 |      567,244,554.97 |    0.1% |      0.01 | `stronk_double_t / stronk_double_t`
|   100.0% |                1.74 |      576,190,563.02 |    0.5% |      0.01 | `int64_t / double`
|   100.1% |                1.73 |      576,617,160.55 |    0.1% |      0.01 | `stronk_int64_t / stronk_double_t`
|   100.0% |                1.74 |      574,671,308.89 |    0.3% |      0.01 | `double / int64_t`
|   100.7% |                1.73 |      578,684,575.97 |    0.4% |      0.01 | `stronk_double_t / stronk_int64_t`

### Notes
- GCC-16 and MSVC shows weak SIMD performance we are investigating why in the following issues:
- https://github.com/twig-energy/stronk/issues/24
- https://github.com/twig-energy/stronk/issues/67

# Licensing

See the [LICENSE](LICENSE) document.
