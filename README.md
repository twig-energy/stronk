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

Stronk is a close to zero cost abstraction - performance varies per compiler and we get the best results when running with clang-13. Unfortunately the performance with MSVC is quite bad. We are investigating the [issue](https://github.com/twig-energy/stronk/issues/24), and initial results points to padding of the stronk structures being the root cause. You can see benchmark results for all the tested platforms in the [Continuous Integration Workflow](https://github.com/twig-energy/stronk/actions/workflows/ci.yml).

Constructing and copying the structs performs identically or very close to identically with just passing the raw types:

```bash
2025-06-09T17:31:42
Running /home/runner/work/stronk/stronk/build/benchmarks/stronk_benchmarks
Run on (4 X 3253.74 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 1.19, 0.36, 0.12
------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                    Time             CPU   Iterations
------------------------------------------------------------------------------------------------------------------------------
benchmark_default_onto_reserved_vector<int8_t>/32                                         12.0 ns         12.0 ns     62407326
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>/32                           2.80 ns         2.80 ns    250149993
benchmark_default_onto_reserved_vector<int8_t>/8192                                       2554 ns         2554 ns       274149
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>/8192                          115 ns          115 ns      6088976

benchmark_default_onto_reserved_vector<int64_t>/32                                        11.2 ns         11.2 ns     62446873
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>/32                          10.9 ns         10.9 ns     64236385
benchmark_default_onto_reserved_vector<int64_t>/8192                                      2557 ns         2557 ns       270881
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>/8192                        2566 ns         2566 ns       272682

benchmark_default_onto_reserved_vector<std::string>/32                                    68.9 ns         68.9 ns     10172651
benchmark_default_onto_reserved_vector<string_wrapping_type>/32                           69.0 ns         69.0 ns     10181646
benchmark_default_onto_reserved_vector<std::string>/8192                                 18293 ns        18292 ns        38252
benchmark_default_onto_reserved_vector<string_wrapping_type>/8192                        18689 ns        18689 ns        38048

benchmark_rand_onto_reserved_vector<int8_t>/32                                             175 ns          175 ns      4029369
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>/32                               183 ns          183 ns      3818736
benchmark_rand_onto_reserved_vector<int8_t>/8192                                         42324 ns        42317 ns        16605
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>/8192                           44696 ns        44692 ns        15645

benchmark_rand_onto_reserved_vector<int64_t>/32                                            507 ns          507 ns      1382460
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>/32                              519 ns          519 ns      1399287
benchmark_rand_onto_reserved_vector<int64_t>/8192                                       130622 ns       130616 ns         5498
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>/8192                         125446 ns       125435 ns         4605

benchmark_rand_onto_reserved_vector<std::string>/32                                      10403 ns        10403 ns        67340
benchmark_rand_onto_reserved_vector<string_wrapping_type>/32                             10645 ns        10644 ns        65943
benchmark_rand_onto_reserved_vector<std::string>/8192                                  2668895 ns      2668719 ns          260
benchmark_rand_onto_reserved_vector<string_wrapping_type>/8192                         2724441 ns      2724121 ns          257

benchmark_copy_vector_of<int8_t>/32                                                       20.1 ns         20.1 ns     34701524
benchmark_copy_vector_of<int8_t_wrapping_type>/32                                         19.9 ns         19.9 ns     34918031
benchmark_copy_vector_of<int8_t>/8192                                                     2746 ns         2746 ns       254751
benchmark_copy_vector_of<int8_t_wrapping_type>/8192                                       2747 ns         2747 ns       255140

benchmark_copy_vector_of<int64_t>/32                                                      30.0 ns         30.0 ns     24108558
benchmark_copy_vector_of<int64_t_wrapping_type>/32                                        30.2 ns         30.2 ns     23655573
benchmark_copy_vector_of<int64_t>/8192                                                   22400 ns        22398 ns        31273
benchmark_copy_vector_of<int64_t_wrapping_type>/8192                                     22364 ns        22363 ns        31317

benchmark_copy_vector_of<std::string>/32                                                   627 ns          627 ns      1124125
benchmark_copy_vector_of<string_wrapping_type>/32                                          633 ns          633 ns      1105766
benchmark_copy_vector_of<std::string>/8192                                              240597 ns       240586 ns         2908
benchmark_copy_vector_of<string_wrapping_type>/8192                                     241608 ns       241588 ns         2885
```

Calling "Skill" functions (which internally calls unwrap) performs identically to calling the functions directly on the raw types. They even auto-vectorize the same:

```
2025-06-09T17:31:42
Running /home/runner/work/stronk/stronk/build/benchmarks/stronk_benchmarks
Run on (4 X 3253.74 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 1.19, 0.36, 0.12
------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                    Time             CPU   Iterations
------------------------------------------------------------------------------------------------------------------------------
benchmark_add_units<int8_t>/8192                                                          5106 ns         5106 ns       137161
benchmark_add_units<int8_t_wrapping_type>/8192                                            2558 ns         2557 ns       267810

benchmark_add_units<int64_t>/8192                                                         2627 ns         2627 ns       262084
benchmark_add_units<int64_t_wrapping_type>/8192                                           2611 ns         2611 ns       269788

benchmark_add_units<double>/8192                                                          2560 ns         2560 ns       263402
benchmark_add_units<double_wrapping_type>/8192                                            2597 ns         2596 ns       261914

benchmark_add_units_simd<int8_t, 32>/256                                                   294 ns          294 ns      2385544
benchmark_add_units_simd<int8_t_wrapping_type, 32>/256                                     292 ns          292 ns      2396000

benchmark_add_units_simd<int64_t, 32>/256                                                 4926 ns         4926 ns       147322
benchmark_add_units_simd<int64_t_wrapping_type, 32>/256                                   4757 ns         4756 ns       147417

benchmark_add_units_simd<double, 32>/256                                                  4817 ns         4817 ns       145288
benchmark_add_units_simd<double_wrapping_type, 32>/256                                    4844 ns         4843 ns       145286

benchmark_subtract_units<int8_t>/8192                                                     5104 ns         5104 ns       137258
benchmark_subtract_units<int8_t_wrapping_type>/8192                                       2593 ns         2593 ns       263280

benchmark_subtract_units<int64_t>/8192                                                    5316 ns         5316 ns       137355
benchmark_subtract_units<int64_t_wrapping_type>/8192                                      2598 ns         2596 ns       268095

benchmark_subtract_units<double>/8192                                                     2595 ns         2595 ns       269322
benchmark_subtract_units<double_wrapping_type>/8192                                       2601 ns         2601 ns       271730

benchmark_subtract_units_simd<int8_t, 32>/256                                              292 ns          292 ns      2397710
benchmark_subtract_units_simd<int8_t_wrapping_type, 32>/256                                293 ns          293 ns      2400188

benchmark_subtract_units_simd<int64_t, 32>/256                                            4756 ns         4756 ns       146574
benchmark_subtract_units_simd<int64_t_wrapping_type, 32>/256                              4753 ns         4752 ns       146956

benchmark_subtract_units_simd<double, 32>/256                                             4827 ns         4826 ns       145283
benchmark_subtract_units_simd<double_wrapping_type, 32>/256                               4821 ns         4821 ns       145180

benchmark_multiply_units<int8_t, int8_t>/8192                                             5106 ns         5106 ns       137260
benchmark_multiply_units<int8_t_wrapping_type, int8_t_wrapping_type>/8192                 2598 ns         2598 ns       273621

benchmark_multiply_units<int64_t, int64_t>/8192                                           5107 ns         5107 ns       137336
benchmark_multiply_units<int64_t_wrapping_type, int64_t_wrapping_type>/8192               2625 ns         2625 ns       267305

benchmark_multiply_units<double, double>/8192                                             2616 ns         2615 ns       264152
benchmark_multiply_units<double_wrapping_type, double_wrapping_type>/8192                 2715 ns         2715 ns       269888

benchmark_multiply_units<int64_t, double>/8192                                            2588 ns         2587 ns       272344
benchmark_multiply_units<int64_t_wrapping_type, double_wrapping_type>/8192                2635 ns         2634 ns       267335

benchmark_multiply_units<double, int64_t>/8192                                            2589 ns         2589 ns       270346
benchmark_multiply_units<double_wrapping_type, int64_t_wrapping_type>/8192                2586 ns         2586 ns       256974

benchmark_multiply_units_simd<int8_t, int8_t, 32>/256                                     3216 ns         3216 ns       217588
benchmark_multiply_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>/256         3216 ns         3215 ns       217792

benchmark_multiply_units_simd<int64_t, int64_t, 32>/256                                   7036 ns         7035 ns        99638
benchmark_multiply_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>/256       6973 ns         6972 ns       100436

benchmark_multiply_units_simd<double, double, 32>/256                                     4818 ns         4818 ns       145227
benchmark_multiply_units_simd<double_wrapping_type, double_wrapping_type, 32>/256         4821 ns         4820 ns       145119

benchmark_multiply_units_simd<int64_t, double, 32>/256                                    5939 ns         5939 ns       118051
benchmark_multiply_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>/256        5940 ns         5939 ns       117936

benchmark_multiply_units_simd<double, int64_t, 32>/256                                    5940 ns         5940 ns       117885
benchmark_multiply_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>/256        5954 ns         5953 ns       117853

benchmark_divide_units<int8_t, int8_t>/8192                                               2603 ns         2603 ns       264947
benchmark_divide_units<int8_t_wrapping_type, int8_t_wrapping_type>/8192                   2655 ns         2655 ns       263236

benchmark_divide_units<int64_t, int64_t>/8192                                             2741 ns         2741 ns       259965
benchmark_divide_units<int64_t_wrapping_type, int64_t_wrapping_type>/8192                 2582 ns         2581 ns       265405

benchmark_divide_units<double, double>/8192                                               2631 ns         2631 ns       268084
benchmark_divide_units<double_wrapping_type, double_wrapping_type>/8192                   2641 ns         2640 ns       273748

benchmark_divide_units<int64_t, double>/8192                                              2608 ns         2608 ns       264458
benchmark_divide_units<int64_t_wrapping_type, double_wrapping_type>/8192                  2618 ns         2617 ns       263916

benchmark_divide_units<double, int64_t>/8192                                              2559 ns         2559 ns       268442
benchmark_divide_units<double_wrapping_type, int64_t_wrapping_type>/8192                  2584 ns         2584 ns       273500

benchmark_divide_units_simd<int8_t, int8_t, 32>/256                                      13372 ns        13371 ns        52398
benchmark_divide_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>/256          13361 ns        13360 ns        52344

benchmark_divide_units_simd<int64_t, int64_t, 32>/256                                    15750 ns        15748 ns        44440
benchmark_divide_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>/256        15749 ns        15748 ns        44459

benchmark_divide_units_simd<double, double, 32>/256                                       5730 ns         5730 ns       125709
benchmark_divide_units_simd<double_wrapping_type, double_wrapping_type, 32>/256           5712 ns         5712 ns       123217

benchmark_divide_units_simd<int64_t, double, 32>/256                                     10051 ns        10050 ns        69697
benchmark_divide_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>/256         10050 ns        10049 ns        69550

benchmark_divide_units_simd<double, int64_t, 32>/256                                     10041 ns        10040 ns        69773
benchmark_divide_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>/256         10099 ns        10098 ns        69731
```

# Licensing

See the [LICENSE](LICENSE) document.
