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

struct FirstName : twig::stronk<FirstName, std::string, twig::can_ostream>
{
    using stronk::stronk;
};
struct LastName : twig::stronk<LastName, std::string>
{
    using stronk::stronk;
};

// Strong types protects you from accidentally passing the wrong argument to the wrong parameter.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    // The twig::can_ostream skill overloads the `operator<<(ostream&)` for your type.
    std::cout << firstname << " ";
    // You can also access the underlying type by using the .unwrap<Type>() function.
    std::cout << lastname.unwrap<LastName>() << std::endl;
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
- `can_add`: binary `operator+` `operator=+`
- `can_subtract`: binary `operator-` and `operator=-`
- `can_multiply`: binary `operator*` and `operator=*` (not compatible with units, we encourage you to use units instead)
- `can_divide`: binary `operator/` and `operator=/` (not compatible with units, we encourage you to use units instead)
- `can_abs`: overloads `twig::abs`
- `can_isnan`: overloads `twig::isnan`
- `can_stream`: overloads `operator<<(std::ostream)` and `operator<<(std::istream)`, stream the underlying value to the stream, or create from stream. For only `ostream` or `istream` functionality, use `can_ostream` or `can_istream` respectively.
- `can_order`: `operator<=>`, note you probably also want to add `can_equate`, since the compiler cannot generate equality with the `operator<=>` for stronk types.
- `can_equate`: `operator==` with regular equality
- `can_equate_with_is_close`: `operator==` but with numpy's `is_close` definition of equal
- `can_equate_with_is_close_nan_equals`: `operator==` but with numpy's `is_close` definition of equal, nans being equal
- `can_equate_with_is_close_abs_tol_only`: `operator==` with a small absolute tolerance for difference
- `can_less_than_greater_than`: `operator<` and `operator>` (prefer the `can_order` skill instead)
- `can_less_than_greater_than_or_equal`: operator <= and operator >= (prefer the `can_order` skill instead)
- `can_be_used_as_flag`: for boolean values used as flags
- `can_hash`: specializes `std::hash<T>`.
- `can_size`: implements `.size()` and `.empty()`
- `can_const_iterate` implements `begin() const`, `end() const`, `cbegin() const` and `cend() const`.
- `can_iterate` adds the `can_const_iterate` as well implementing `begin()`, `end()`.
- `can_const_index` implements `operator[](const auto&) const` and `at(const auto&) const`
- `can_index` adds the `can_const_index` as well implementing `operator[](const auto&)` and `at(const auto&)`.
- `can_increment` adds both `operator++` operators.
- `can_decrement` adds both `operator--` operators.

### Third Party Library extensions (see `stronk/extensions/<library>.hpp`)

- `can_absl_hash`: implements the `AbslHashValue` friend function.
- `can_gtest_print`: for printing the values in gtest check macros
- `can_fmt_format`: implements `struct fmt::formatter<T>` with default formatting string `"{}"`. In the future we will add a `can_format` for `std::format`.
- `can_fmt_format_builder<"fmt format string{}">::skill`: implements `struct fmt::formatter<T>`. In the future we will add a `can_format_builder<"std format string">` for `std::format`.

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

Stronk is a close to zero cost abstraction - performance varies per compiler and we get the best results when running with gcc-14. Unfortunately the performance with MSVC is quite bad. We are investigating the [issue](https://github.com/twig-energy/stronk/issues/24), and initial results points to padding of the stronk structures being the root cause. You can see benchmark results for all the tested platforms in the [Continuous Integration Workflow](https://github.com/twig-energy/stronk/actions/workflows/ci.yml).

Constructing and copying the structs performs identically or very close to identically with just passing the raw types:

```bash

| relative |               ns/op |                op/s |    err% |     total | Default Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------------------------------
|   100.0% |                1.35 |      740,711,218.87 |    0.4% |      0.01 | `int8_t`
|10,982.4% |                0.01 |   81,348,012,975.29 |    0.1% |      0.01 | `int8_t_wrapping_type`
|   100.0% |                0.31 |    3,209,582,423.68 |    0.1% |      0.01 | `int64_t`
|    99.8% |                0.31 |    3,203,516,330.00 |    0.1% |      0.01 | `int64_t_wrapping_type`
|   100.0% |                3.49 |      286,495,339.90 |    0.5% |      0.01 | `std::string`
|   100.4% |                3.48 |      287,695,250.94 |    0.3% |      0.01 | `string_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Random Construction onto Reserved Vector
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------------------------
|   100.0% |                5.45 |      183,513,493.49 |    0.0% |      0.01 | `int8_t`
|   100.1% |                5.44 |      183,704,061.48 |    0.1% |      0.01 | `int8_t_wrapping_type`
|   100.0% |               15.58 |       64,191,634.60 |    0.1% |      0.01 | `int64_t`
|   100.0% |               15.57 |       64,220,814.80 |    0.1% |      0.01 | `int64_t_wrapping_type`
|   100.0% |              337.97 |        2,958,821.38 |    0.3% |      0.03 | `std::string`
|   100.5% |              336.36 |        2,972,989.06 |    0.3% |      0.03 | `string_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Copy Vector Benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:-----------------------
|   100.0% |                0.33 |    2,989,469,667.87 |    0.1% |      0.01 | `int8_t`
|    99.9% |                0.33 |    2,986,563,973.33 |    0.2% |      0.01 | `int8_t_wrapping_type`
|   100.0% |                2.71 |      368,805,320.99 |    0.1% |      0.01 | `int64_t`
|   100.2% |                2.71 |      369,395,695.26 |    0.3% |      0.01 | `int64_t_wrapping_type`
|   100.0% |               35.67 |       28,037,063.32 |    0.3% |      0.01 | `std::string`
|   100.4% |               35.53 |       28,142,740.58 |    0.2% |      0.01 | `string_wrapping_type`
```

Calling "Skill" functions (which internally calls unwrap) performs identically to calling the functions directly on the raw types. They even auto-vectorize the same:

```

| relative |               ns/op |                op/s |    err% |     total | Add Units
|---------:|--------------------:|--------------------:|--------:|----------:|:----------
|   100.0% |                1.09 |      918,547,216.25 |    1.1% |      0.01 | `int8_t + int8_t`
|   100.8% |                1.08 |      925,465,732.99 |    0.7% |      0.01 | `int8_t_wrapping_type + int8_t_wrapping_type`
|   100.0% |                1.14 |      879,567,144.37 |    0.8% |      0.01 | `int64_t + int64_t`
|   101.0% |                1.13 |      888,444,778.56 |    0.4% |      0.01 | `int64_t_wrapping_type + int64_t_wrapping_type`
|   100.0% |                1.16 |      862,908,076.97 |    0.2% |      0.01 | `double + double`
|   100.6% |                1.15 |      867,891,078.18 |    0.2% |      0.01 | `double_wrapping_type + double_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Add Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |               17.02 |       58,740,331.10 |    0.1% |      0.01 | `int8_t + int8_t`
|   652.6% |                2.61 |      383,332,910.89 |    0.2% |      0.01 | `int8_t_wrapping_type + int8_t_wrapping_type`
|   100.0% |               24.99 |       40,021,222.19 |    0.0% |      0.01 | `int64_t + int64_t`
|    98.5% |               25.36 |       39,431,402.26 |    0.0% |      0.01 | `int64_t_wrapping_type + int64_t_wrapping_type`
|   100.0% |               25.36 |       39,434,097.59 |    0.3% |      0.01 | `double + double`
|   101.2% |               25.06 |       39,911,783.53 |    0.0% |      0.01 | `double_wrapping_type + double_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units
|---------:|--------------------:|--------------------:|--------:|----------:|:---------------
|   100.0% |                1.09 |      916,284,928.30 |    1.6% |      0.01 | `int8_t - int8_t`
|   100.6% |                1.09 |      921,615,806.78 |    0.7% |      0.01 | `int8_t_wrapping_type - int8_t_wrapping_type`
|   100.0% |                1.13 |      888,585,001.14 |    0.4% |      0.01 | `int64_t - int64_t`
|   100.4% |                1.12 |      892,292,956.52 |    0.1% |      0.01 | `int64_t_wrapping_type - int64_t_wrapping_type`
|   100.0% |                1.16 |      861,364,564.51 |    0.4% |      0.01 | `double - double`
|   100.8% |                1.15 |      867,941,420.31 |    0.2% |      0.01 | `double_wrapping_type - double_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Subtract Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:--------------------
|   100.0% |               17.03 |       58,736,645.87 |    0.1% |      0.01 | `int8_t - int8_t`
|   650.4% |                2.62 |      382,004,424.80 |    0.4% |      0.01 | `int8_t_wrapping_type - int8_t_wrapping_type`
|   100.0% |               24.96 |       40,059,619.98 |    0.1% |      0.01 | `int64_t - int64_t`
|    98.5% |               25.35 |       39,451,227.26 |    0.1% |      0.01 | `int64_t_wrapping_type - int64_t_wrapping_type`
|   100.0% |               25.29 |       39,548,895.41 |    0.1% |      0.01 | `double - double`
|   100.9% |               25.06 |       39,906,741.37 |    0.0% |      0.01 | `double_wrapping_type - double_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | multiply_units_benchmarks
|---------:|--------------------:|--------------------:|--------:|----------:|:--------------------------
|   100.0% |                1.07 |      935,659,443.04 |    0.2% |      0.01 | `int8_t * int8_t`
|   100.1% |                1.07 |      936,973,688.60 |    0.1% |      0.01 | `int8_t_wrapping_type * int8_t_wrapping_type`
|   100.0% |                1.14 |      876,992,986.35 |    2.8% |      0.01 | `int64_t * int64_t`
|   102.6% |                1.11 |      900,088,684.10 |    0.5% |      0.01 | `int64_t_wrapping_type * int64_t_wrapping_type`
|   100.0% |                1.18 |      847,724,492.35 |    1.1% |      0.01 | `double * double`
|   101.6% |                1.16 |      861,177,889.60 |    0.9% |      0.01 | `double_wrapping_type * double_wrapping_type`
|   100.0% |                1.16 |      858,816,905.78 |    0.7% |      0.01 | `int64_t * double`
|   103.9% |                1.12 |      892,148,795.72 |    0.3% |      0.01 | `int64_t_wrapping_type * double_wrapping_type`
|   100.0% |                1.16 |      864,198,534.34 |    0.5% |      0.01 | `double * int64_t`
|   100.8% |                1.15 |      871,442,195.18 |    0.0% |      0.01 | `double_wrapping_type * int64_t_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Multiply Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:--------------------
|   100.0% |               17.01 |       58,775,732.77 |    0.1% |      0.01 | `int8_t * int8_t`
|   112.2% |               15.17 |       65,928,737.49 |    0.1% |      0.01 | `int8_t_wrapping_type * int8_t_wrapping_type`
|   100.0% |               32.27 |       30,991,241.15 |    0.0% |      0.01 | `int64_t * int64_t`
|    99.8% |               32.32 |       30,939,652.19 |    0.2% |      0.01 | `int64_t_wrapping_type * int64_t_wrapping_type`
|   100.0% |               25.01 |       39,984,348.57 |    0.1% |      0.01 | `double * double`
|    99.8% |               25.05 |       39,913,741.10 |    0.1% |      0.01 | `double_wrapping_type * double_wrapping_type`
|   100.0% |               30.73 |       32,546,133.01 |    0.1% |      0.01 | `int64_t * double`
|    93.5% |               32.85 |       30,445,439.22 |    0.2% |      0.01 | `int64_t_wrapping_type * double_wrapping_type`
|   100.0% |               30.74 |       32,531,237.65 |    0.1% |      0.01 | `double * int64_t`
|   100.0% |               30.73 |       32,540,315.41 |    0.1% |      0.01 | `double_wrapping_type * int64_t_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Divide Units
|---------:|--------------------:|--------------------:|--------:|----------:|:-------------
|   100.0% |                1.86 |      537,004,260.90 |    0.1% |      0.01 | `int8_t / int8_t`
|   100.0% |                1.86 |      537,010,040.37 |    0.0% |      0.01 | `int8_t_wrapping_type / int8_t_wrapping_type`
|   100.0% |                2.17 |      460,303,190.01 |    0.0% |      0.01 | `int64_t / int64_t`
|   100.0% |                2.17 |      460,258,600.98 |    0.1% |      0.01 | `int64_t_wrapping_type / int64_t_wrapping_type`
|   100.0% |                1.40 |      714,971,460.49 |    0.1% |      0.01 | `double / double`
|   100.0% |                1.40 |      714,967,119.62 |    0.1% |      0.01 | `double_wrapping_type / double_wrapping_type`
|   100.0% |                1.40 |      714,839,057.47 |    0.1% |      0.01 | `int64_t / double`
|   100.0% |                1.40 |      714,811,335.30 |    0.1% |      0.01 | `int64_t_wrapping_type / double_wrapping_type`
|   100.0% |                1.40 |      714,812,479.96 |    0.1% |      0.01 | `double / int64_t`
|   100.0% |                1.40 |      714,777,440.86 |    0.1% |      0.01 | `double_wrapping_type / int64_t_wrapping_type`

| relative |               ns/op |                op/s |    err% |     total | Divide Units SIMD
|---------:|--------------------:|--------------------:|--------:|----------:|:------------------
|   100.0% |               59.39 |       16,837,881.95 |    0.0% |      0.01 | `int8_t / int8_t`
|   100.0% |               59.40 |       16,833,902.89 |    0.0% |      0.01 | `int8_t_wrapping_type / int8_t_wrapping_type`
|   100.0% |               73.92 |       13,528,331.55 |    0.1% |      0.01 | `int64_t / int64_t`
|   100.0% |               73.96 |       13,521,621.47 |    0.1% |      0.01 | `int64_t_wrapping_type / int64_t_wrapping_type`
|   100.0% |               29.66 |       33,714,674.34 |    0.2% |      0.01 | `double / double`
|   100.7% |               29.47 |       33,938,540.25 |    0.1% |      0.01 | `double_wrapping_type / double_wrapping_type`
|   100.0% |               55.23 |       18,107,348.82 |    0.4% |      0.01 | `int64_t / double`
|    97.0% |               56.95 |       17,559,258.75 |    0.2% |      0.01 | `int64_t_wrapping_type / double_wrapping_type`
|   100.0% |               55.70 |       17,954,065.03 |    0.3% |      0.01 | `double / int64_t`
|   100.3% |               55.55 |       18,002,628.31 |    0.2% |      0.01 | `double_wrapping_type / int64_t_wrapping_type`
```

# Licensing

See the [LICENSE](LICENSE) document.
