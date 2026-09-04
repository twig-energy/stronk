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

Stronk is a close to zero cost abstraction - performance varies per compiler.
You can see benchmark results for all the tested platforms in the [Continuous Integration Workflow](https://github.com/twig-energy/stronk/actions/workflows/ci.yml).

| relative |               ns/op |                op/s |    err% |     total | Default Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------------------------
|   100.0% |                0.41 |    2,421,113,737.12 |    0.4% |      0.01 | `int8_t`
|    99.4% |                0.42 |    2,407,504,610.51 |    1.1% |      0.01 | `stronk_int8_t`
|   100.0% |                0.07 |   14,095,550,602.57 |    0.2% |      0.01 | `int64_t`
|    14.7% |                0.48 |    2,073,833,840.80 |    0.1% |      0.01 | `stronk_int64_t`
|   100.0% |                1.89 |      528,491,622.36 |    0.5% |      0.01 | `std::string`
|    99.8% |                1.90 |      527,202,436.19 |    0.3% |      0.01 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Random Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------------------------
|   100.0% |                4.01 |      249,119,843.74 |    0.5% |      0.01 | `int8_t`
|   100.4% |                4.00 |      250,159,580.76 |    0.1% |      0.01 | `stronk_int8_t`
|   100.0% |               13.80 |       72,457,600.88 |    0.1% |      0.01 | `int64_t`
|    97.4% |               14.16 |       70,603,282.29 |    0.3% |      0.01 | `stronk_int64_t`
|   100.0% |              264.20 |        3,784,970.08 |    0.2% |      0.02 | `std::string`
|   100.1% |              264.05 |        3,787,180.08 |    0.3% |      0.02 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Copy Vector Benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------
|   100.0% |              169.93 |        5,884,825.15 |    0.3% |      0.01 | `int8_t`
|   100.6% |              168.96 |        5,918,404.11 |    0.2% |      0.01 | `stronk_int8_t`
|   100.0% |            1,162.08 |          860,523.69 |    0.1% |      0.01 | `int64_t`
|   100.1% |            1,161.35 |          861,063.55 |    0.1% |      0.01 | `stronk_int64_t`
|   100.0% |           38,399.12 |           26,042.26 |    0.2% |      0.01 | `std::string`
|   100.0% |           38,386.75 |           26,050.66 |    0.1% |      0.01 | `string_stronk_t`

| relative |               ns/op |                op/s |    err% |     total | Add Units
|---------:|--------------------:|--------------------:|--------:|----------:|:----------
|   100.0% |                0.91 |    1,104,478,972.78 |    0.2% |      0.01 | `int8_t + int8_t`
|    91.4% |                0.99 |    1,009,863,863.02 |    0.1% |      0.01 | `stronk_int8_t + stronk_int8_t`
|   100.0% |                0.99 |    1,010,994,393.00 |    0.0% |      0.01 | `int64_t + int64_t`
|    95.4% |                1.04 |      964,989,864.58 |    0.2% |      0.01 | `stronk_int64_t + stronk_int64_t`
|   100.0% |                0.91 |    1,100,096,354.88 |    0.2% |      0.01 | `double + double`
|    92.9% |                0.98 |    1,022,027,805.72 |    0.1% |      0.01 | `stronk_double_t + stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Add Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |                0.45 |    2,206,628,741.22 |    0.2% |      0.01 | `int8_t + int8_t`
|    47.4% |                0.96 |    1,045,931,212.01 |    3.0% |      0.01 | `stronk_int8_t + stronk_int8_t`
|   100.0% |                0.51 |    1,972,214,424.48 |    0.2% |      0.01 | `int64_t + int64_t`
|    50.0% |                1.01 |      986,615,432.18 |    0.1% |      0.01 | `stronk_int64_t + stronk_int64_t`
|   100.0% |                0.49 |    2,054,456,864.43 |    0.1% |      0.01 | `double + double`
|    52.5% |                0.93 |    1,078,415,635.40 |    1.5% |      0.01 | `stronk_double_t + stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |                0.91 |    1,104,666,683.60 |    0.2% |      0.01 | `int8_t - int8_t`
|    88.5% |                1.02 |      978,047,069.71 |    1.7% |      0.01 | `stronk_int8_t - stronk_int8_t`
|   100.0% |                1.00 |    1,003,823,935.58 |    0.5% |      0.01 | `int64_t - int64_t`
|    94.5% |                1.05 |      948,138,320.84 |    0.7% |      0.01 | `stronk_int64_t - stronk_int64_t`
|   100.0% |                0.91 |    1,100,779,360.39 |    0.2% |      0.01 | `double - double`
|    92.8% |                0.98 |    1,021,084,687.19 |    0.2% |      0.01 | `stronk_double_t - stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------
|   100.0% |                0.46 |    2,181,537,567.39 |    0.4% |      0.01 | `int8_t - int8_t`
|    49.8% |                0.92 |    1,085,557,161.61 |    0.1% |      0.01 | `stronk_int8_t - stronk_int8_t`
|   100.0% |                0.50 |    1,981,894,296.86 |    0.1% |      0.01 | `int64_t - int64_t`
|    49.1% |                1.03 |      972,958,727.21 |    0.8% |      0.01 | `stronk_int64_t - stronk_int64_t`
|   100.0% |                0.49 |    2,053,575,780.23 |    0.1% |      0.01 | `double - double`
|    52.2% |                0.93 |    1,071,713,581.75 |    1.6% |      0.01 | `stronk_double_t - stronk_double_t`

| relative |               ns/op |                op/s |    err% |     total | multiply_units_benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:--------------------------
|   100.0% |                0.94 |    1,058,274,138.61 |    0.3% |      0.01 | `int8_t * int8_t`
|    89.9% |                1.05 |      951,191,276.68 |    1.2% |      0.01 | `stronk_int8_t * stronk_int8_t`
|   100.0% |                0.95 |    1,052,160,657.60 |    0.1% |      0.01 | `int64_t * int64_t`
|    91.8% |                1.04 |      965,658,452.01 |    0.1% |      0.01 | `stronk_int64_t * stronk_int64_t`
|   100.0% |                0.91 |    1,099,982,124.62 |    0.1% |      0.01 | `double * double`
|    92.1% |                0.99 |    1,012,999,459.00 |    1.0% |      0.01 | `stronk_double_t * stronk_double_t`
|   100.0% |                0.95 |    1,055,468,731.70 |    1.2% |      0.01 | `int64_t * double`
|    96.4% |                0.98 |    1,017,923,361.08 |    0.2% |      0.01 | `stronk_int64_t * stronk_double_t`
|   100.0% |                0.92 |    1,087,366,477.02 |    0.1% |      0.01 | `double * int64_t`
|    92.8% |                0.99 |    1,009,063,900.11 |    0.7% |      0.01 | `stronk_double_t * stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Multiply Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------
|   100.0% |                0.54 |    1,864,542,799.92 |    2.2% |      0.01 | `int8_t * int8_t`
|    54.3% |                0.99 |    1,012,313,557.37 |    0.1% |      0.01 | `stronk_int8_t * stronk_int8_t`
|   100.0% |                0.49 |    2,023,579,079.74 |    0.2% |      0.01 | `int64_t * int64_t`
|    46.6% |                1.06 |      942,899,643.48 |    0.1% |      0.01 | `stronk_int64_t * stronk_int64_t`
|   100.0% |                0.49 |    2,048,699,462.69 |    0.3% |      0.01 | `double * double`
|    51.0% |                0.96 |    1,045,841,206.83 |    2.6% |      0.01 | `stronk_double_t * stronk_double_t`
|   100.0% |                0.48 |    2,072,819,189.63 |    0.1% |      0.01 | `int64_t * double`
|    49.4% |                0.98 |    1,024,408,162.63 |    0.0% |      0.01 | `stronk_int64_t * stronk_double_t`
|   100.0% |                0.48 |    2,067,939,710.98 |    0.1% |      0.01 | `double * int64_t`
|    49.5% |                0.98 |    1,023,675,179.99 |    0.1% |      0.01 | `stronk_double_t * stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Divide Units
|---------:|--------------------:|--------------------:|--------:|----------:|:-------------
|   100.0% |                1.15 |      868,567,613.87 |    0.5% |      0.01 | `int8_t / int8_t`
|   100.7% |                1.14 |      874,412,229.01 |    0.2% |      0.01 | `stronk_int8_t / stronk_int8_t`
|   100.0% |                1.91 |      524,338,336.48 |    0.0% |      0.01 | `int64_t / int64_t`
|   100.0% |                1.91 |      524,328,812.62 |    0.0% |      0.01 | `stronk_int64_t / stronk_int64_t`
|   100.0% |                1.28 |      782,300,995.59 |    1.4% |      0.01 | `double / double`
|   100.6% |                1.27 |      787,012,708.41 |    0.7% |      0.01 | `stronk_double_t / stronk_double_t`
|   100.0% |                1.30 |      768,764,396.42 |    0.6% |      0.01 | `int64_t / double`
|   100.3% |                1.30 |      770,995,561.03 |    0.2% |      0.01 | `stronk_int64_t / stronk_double_t`
|   100.0% |                1.28 |      782,191,944.99 |    0.3% |      0.01 | `double / int64_t`
|    99.8% |                1.28 |      780,882,528.17 |    0.4% |      0.01 | `stronk_double_t / stronk_int64_t`

| relative |               ns/op |                op/s |    err% |     total | Divide Units SIMD<32>
|---------:|--------------------:|--------------------:|--------:|----------:|:----------------------
|   100.0% |                2.11 |      475,018,337.96 |    0.1% |      0.01 | `int8_t / int8_t`
|   100.0% |                2.11 |      474,802,214.04 |    0.1% |      0.01 | `stronk_int8_t / stronk_int8_t`
|   100.0% |                1.91 |      524,734,246.72 |    0.1% |      0.01 | `int64_t / int64_t`
|    99.9% |                1.91 |      524,264,172.57 |    0.1% |      0.01 | `stronk_int64_t / stronk_int64_t`
|   100.0% |                1.36 |      734,675,235.46 |    0.1% |      0.01 | `double / double`
|   100.0% |                1.36 |      735,040,798.44 |    0.1% |      0.01 | `stronk_double_t / stronk_double_t`
|   100.0% |                1.36 |      734,904,486.86 |    0.6% |      0.01 | `int64_t / double`
|   100.0% |                1.36 |      734,658,560.65 |    0.1% |      0.01 | `stronk_int64_t / stronk_double_t`
|   100.0% |                1.36 |      735,295,090.25 |    0.3% |      0.01 | `double / int64_t`
|   100.0% |                1.36 |      735,311,466.16 |    0.2% |      0.01 | `stronk_double_t / stronk_int64_t`


### Notes
- Some benchmarks shows weak SIMD performance we are investigating why in the following issues:
    - https://github.com/twig-energy/stronk/issues/24
    - https://github.com/twig-energy/stronk/issues/67

# Licensing

See the [LICENSE](LICENSE) document.
