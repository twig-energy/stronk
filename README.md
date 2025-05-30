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

```cpp :file=./examples/unit_energy_example.cpp:line_start=0:line_end=26
#include <concepts>
#include <ratio>

#include <stronk/stronk.hpp>
#include <stronk/unit.hpp>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct joules_unit : twig::stronk_default_unit<joules_unit, std::ratio<1>>
{
};

template<typename T>
using joules = joules_unit::value<T>;

void joules_and_identity_units()
{
    auto energy = joules<double> {30.};
    energy += joules<double> {4.} - joules<double> {2.};  // we can add and subtract units

    // Multiplying and dividing with an identity_unit (such as floats and integers) does not change the type.
    energy *= 2.;

    // However an identity_unit divided by a regular unit results in a new unit type.
    auto one_over_joules = 1.0 / energy;
    static_assert(!std::same_as<decltype(one_over_joules), joules<double>>);
}
```

Different units can be combined by multiplying or dividing them:

```cpp :file=./examples/unit_energy_example.cpp:line_start=28:line_end=63
struct seconds_unit : twig::stronk_default_unit<seconds_unit, std::ratio<1>>
{
};

template<typename T>
using seconds = seconds_unit::value<T>;

// We can define ratios of a specific unit - these scaled units have the same dimension
template<typename T>
using hours = seconds_unit::scaled_t<std::ratio<60 * 60>>::value<T>;

// We can now dynamically generate a new type!
using watt_unit = twig::divided_unit_t<joules_unit, seconds_unit>;

template<typename T>
using watt = watt_unit::value<T>;

// or make custom names for already known types (joules) with specific scale
using watt_hours_unit = decltype(watt<double> {} * hours<double> {})::unit_t;

template<typename T>
using watt_hours = watt_hours_unit::value<T>;

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    watt_hours<double> watt_hours_val = hours<double> {3.} * watt<double> {25.};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours_val -= watt_hours<double> {10.} + watt_hours<double> {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    hours<double> hours_val = watt_hours_val / watt<double> {25.};
    watt<double> watt_val = watt_hours_val / hours<double> {3.};
}
```

These new generated types are also units which can be used to generate new units:

```cpp :file=./examples/unit_energy_example.cpp:line_start=65:line_end=89
struct euro_unit : twig::stronk_default_unit<euro_unit, std::ratio<1>>
{
};
template<typename T>
using euro = euro_unit::value<T>;

template<typename T>
using mega_watt_hours =
    joules_unit::scaled_t<std::ratio_multiply<std::mega, typename watt_hours_unit::scale_t>>::value<T>;

void introducing_another_type()
{
    // twig::make allows you to scale the input value but it does not change the resulting type
    mega_watt_hours<double> one_mega_watt_hour = mega_watt_hours<double> {1.};
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_mega_watt_hour = euro<double> {300.} / one_mega_watt_hour;

    // This flexibility allows us to write expessive code, while having the type system check our implementation.
    euro<double> price_for_buying_5_mega_watt_hours =
        euros_per_mega_watt_hour * (twig::identity_value_t<std::mega, double> {1} * watt_hours<double> {5.});

    auto mega_watt_hours_per_euro = 1. / euros_per_mega_watt_hour;  // `(Watt * Hours) / Euro`
    mega_watt_hours<double> mega_watt_hours_affordable_for_500_euros = mega_watt_hours_per_euro * euro<double> {500.};
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

- `stronk_flag`: flag like boolean with equal operators etc.

## Examples

### Specializers

Specialization of unit multiplication and division is possible

By default the units are generated with the `stronk_default_unit` type.

```cpp :file=./examples/specializers_example.cpp:line_end=33
#include <concepts>
#include <cstdint>
#include <ratio>

#include "stronk/unit.hpp"

// Let's consider the following units:
struct meters_unit : twig::unit<meters_unit, std::ratio<1>>
{
};

struct seconds_unit : twig::unit<seconds_unit, std::ratio<1>>
{
};

// Let's say you want to use a custom defined stronk type for certain unit combinations.
// Let's introduce our own `Speed` type:
struct meters_per_second_unit : twig::unit<twig::divided_dimensions_t<meters_unit, seconds_unit>, std::ratio<1>>
{
};
// Notice we are using twig::divided_dimensions_t instead of the regular tag

// To make it possible for stronk to find this type we need to specialize `unit_lookup`:
template<>
struct twig::unit_lookup<twig::divided_dimensions_t<meters_unit, seconds_unit>>
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

```
2022-09-06T15:29:32+02:00
Running ./build/benchmarks/stronk_benchmarks
Run on (64 X 4549.12 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x32)
  L1 Instruction 32 KiB (x32)
  L2 Unified 512 KiB (x32)
  L3 Unified 16384 KiB (x8)
Load Average: 5.12, 10.46, 10.65
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                    Time             CPU   Iterations
------------------------------------------------------------------------------------------------------------------------------
benchmark_default_onto_reserved_vector<int8_t>                  /32                       8.91 ns         8.91 ns     78563433
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>    /32                       8.66 ns         8.66 ns     80918037
benchmark_default_onto_reserved_vector<int8_t>                  /8192                     1980 ns         1979 ns       353087
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>    /8192                     1986 ns         1986 ns       354193

benchmark_default_onto_reserved_vector<int64_t>                 /32                       8.90 ns         8.90 ns     78708349
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>   /32                       9.09 ns         9.09 ns     80903274
benchmark_default_onto_reserved_vector<int64_t>                 /8192                     2045 ns         2044 ns       343621
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>   /8192                     2037 ns         2036 ns       343414

benchmark_default_onto_reserved_vector<std::string>             /32                       47.8 ns         47.8 ns     14663125
benchmark_default_onto_reserved_vector<string_wrapping_type>    /32                       85.5 ns         85.5 ns      8131930
benchmark_default_onto_reserved_vector<std::string>             /8192                    19631 ns        19624 ns        36104
benchmark_default_onto_reserved_vector<string_wrapping_type>    /8192                    22198 ns        22190 ns        31634


benchmark_rand_onto_reserved_vector<int8_t>                     /32                        158 ns          158 ns      4422920
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>       /32                        161 ns          161 ns      4321957
benchmark_rand_onto_reserved_vector<int8_t>                     /8192                    39296 ns        39287 ns        18003
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>       /8192                    41177 ns        41165 ns        13780

benchmark_rand_onto_reserved_vector<int64_t>                    /32                        492 ns          491 ns      1341406
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>      /32                        474 ns          474 ns      1457581
benchmark_rand_onto_reserved_vector<int64_t>                    /8192                   118668 ns       118618 ns         6019
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>      /8192                   117694 ns       117661 ns         5962

benchmark_rand_onto_reserved_vector<std::string>                /32                      21612 ns        21605 ns        32047
benchmark_rand_onto_reserved_vector<string_wrapping_type>       /32                      21155 ns        21149 ns        32177
benchmark_rand_onto_reserved_vector<std::string>                /8192                  5583745 ns      5581594 ns          126
benchmark_rand_onto_reserved_vector<string_wrapping_type>       /8192                  5520208 ns      5518877 ns          125


benchmark_copy_vector_of<int8_t>                                /32                       11.0 ns         11.0 ns     61066804
benchmark_copy_vector_of<int8_t_wrapping_type>                  /32                       11.8 ns         11.8 ns     58053187
benchmark_copy_vector_of<int8_t>                                /8192                    100.0 ns         99.9 ns      6840525
benchmark_copy_vector_of<int8_t_wrapping_type>                  /8192                    114.0 ns          114 ns      6107469

benchmark_copy_vector_of<int64_t>                               /32                       11.8 ns         11.8 ns     59132687
benchmark_copy_vector_of<int64_t_wrapping_type>                 /32                       13.0 ns         13.0 ns     54934171
benchmark_copy_vector_of<int64_t>                               /8192                     1070 ns         1070 ns       653306
benchmark_copy_vector_of<int64_t_wrapping_type>                 /8192                     1067 ns         1067 ns       657525

benchmark_copy_vector_of<std::string>                           /32                        522 ns          522 ns      1336216
benchmark_copy_vector_of<string_wrapping_type>                  /32                        528 ns          528 ns      1319940
benchmark_copy_vector_of<std::string>                           /8192                   206292 ns       206209 ns         3501
benchmark_copy_vector_of<string_wrapping_type>                  /8192                   207750 ns       207709 ns         3368

```

Calling "Skill" functions (which internally calls unwrap) performs identically to calling the functions directly on the raw types. They even auto-vectorize the same:

```
2022-09-06T15:29:32+02:00
Running ./build/benchmarks/stronk_benchmarks
Run on (64 X 4549.12 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x32)
  L1 Instruction 32 KiB (x32)
  L2 Unified 512 KiB (x32)
  L3 Unified 16384 KiB (x8)
Load Average: 5.12, 10.46, 10.65
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
--------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                      Time             CPU   Iterations
--------------------------------------------------------------------------------------------------------------------------------
benchmark_add_units<int8_t>                                                       /8192     4114 ns         4113 ns       172898
benchmark_add_units<int8_t_wrapping_type>                                         /8192     4074 ns         4073 ns       172007

benchmark_add_units<int64_t>                                                      /8192     4059 ns         4058 ns       172825
benchmark_add_units<int64_t_wrapping_type>                                        /8192     4136 ns         4135 ns       172749

benchmark_add_units<double>                                                       /8192     4168 ns         4167 ns       171310
benchmark_add_units<double_wrapping_type>                                         /8192     4203 ns         4202 ns       170063

benchmark_add_units_simd<int8_t, 32>                                              /256       284 ns          284 ns      2482647
benchmark_add_units_simd<int8_t_wrapping_type, 32>                                /256       278 ns          278 ns      2521970

benchmark_add_units_simd<int64_t, 32>                                             /256      1804 ns         1802 ns       386445
benchmark_add_units_simd<int64_t_wrapping_type, 32>                               /256      1802 ns         1801 ns       387243

benchmark_add_units_simd<double, 32>                                              /256      1812 ns         1811 ns       386561
benchmark_add_units_simd<double_wrapping_type, 32>                                /256      1791 ns         1790 ns       388848


benchmark_subtract_units<int8_t>                                                  /8192     4129 ns         4128 ns       172555
benchmark_subtract_units<int8_t_wrapping_type>                                    /8192     4042 ns         4041 ns       172917

benchmark_subtract_units<int64_t>                                                 /8192     4016 ns         4015 ns       174396
benchmark_subtract_units<int64_t_wrapping_type>                                   /8192     4012 ns         4011 ns       174147

benchmark_subtract_units<double>                                                  /8192     4030 ns         4029 ns       173898
benchmark_subtract_units<double_wrapping_type>                                    /8192     4019 ns         4018 ns       174130

benchmark_subtract_units_simd<int8_t, 32>                                         /256       278 ns          278 ns      2528806
benchmark_subtract_units_simd<int8_t_wrapping_type, 32>                           /256       278 ns          278 ns      2522150

benchmark_subtract_units_simd<int64_t, 32>                                        /256      1796 ns         1796 ns       388739
benchmark_subtract_units_simd<int64_t_wrapping_type, 32>                          /256      1797 ns         1797 ns       389213

benchmark_subtract_units_simd<double, 32>                                         /256      1817 ns         1817 ns       386436
benchmark_subtract_units_simd<double_wrapping_type, 32>                           /256      1806 ns         1806 ns       384036


benchmark_multiply_units<int8_t, int8_t>                                          /8192     4840 ns         4840 ns       144495
benchmark_multiply_units<int8_t_wrapping_type, int8_t_wrapping_type>              /8192     4836 ns         4835 ns       144600

benchmark_multiply_units<int64_t, int64_t>                                        /8192     4176 ns         4175 ns       168529
benchmark_multiply_units<int64_t_wrapping_type, int64_t_wrapping_type>            /8192     4144 ns         4144 ns       167954

benchmark_multiply_units<double, double>                                          /8192     4032 ns         4031 ns       173870
benchmark_multiply_units<double_wrapping_type, double_wrapping_type>              /8192     4029 ns         4027 ns       172664

benchmark_multiply_units<int64_t, double>                                         /8192     4044 ns         4043 ns       173192
benchmark_multiply_units<int64_t_wrapping_type, double_wrapping_type>             /8192     4040 ns         4039 ns       173067

benchmark_multiply_units<double, int64_t>                                         /8192     4036 ns         4034 ns       173323
benchmark_multiply_units<double_wrapping_type, int64_t_wrapping_type>             /8192     3990 ns         3989 ns       172120


benchmark_multiply_units_simd<int8_t, int8_t, 32>                                 /256      2162 ns         2161 ns       323603
benchmark_multiply_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>     /256      2168 ns         2167 ns       323478

benchmark_multiply_units_simd<int64_t, int64_t, 32>                               /256      2756 ns         2755 ns       256899
benchmark_multiply_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>   /256      2731 ns         2730 ns       255456

benchmark_multiply_units_simd<double, double, 32>                                 /256      1835 ns         1835 ns       381571
benchmark_multiply_units_simd<double_wrapping_type, double_wrapping_type, 32>     /256      1826 ns         1826 ns       386378

benchmark_multiply_units_simd<int64_t, double, 32>                                /256      3938 ns         3937 ns       181518
benchmark_multiply_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>    /256      3922 ns         3921 ns       174342

benchmark_multiply_units_simd<double, int64_t, 32>                                /256      3979 ns         3978 ns       175656
benchmark_multiply_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>    /256      3973 ns         3972 ns       175886


benchmark_divide_units<int8_t, int8_t>                                            /8192    28624 ns        28615 ns        24461
benchmark_divide_units<int8_t_wrapping_type, int8_t_wrapping_type>                /8192    28646 ns        28639 ns        24440

benchmark_divide_units<int64_t, int64_t>                                          /8192    28736 ns        28725 ns        24393
benchmark_divide_units<int64_t_wrapping_type, int64_t_wrapping_type>              /8192    28449 ns        28444 ns        24369

benchmark_divide_units<double, double>                                            /8192    10116 ns        10114 ns        69634
benchmark_divide_units<double_wrapping_type, double_wrapping_type>                /8192    10175 ns        10173 ns        69232

benchmark_divide_units<int64_t, double>                                           /8192    10280 ns        10277 ns        69438
benchmark_divide_units<int64_t_wrapping_type, double_wrapping_type>               /8192     9991 ns         9988 ns        69697

benchmark_divide_units<double, int64_t>                                           /8192    10079 ns        10077 ns        69349
benchmark_divide_units<double_wrapping_type, int64_t_wrapping_type>               /8192    10043 ns        10038 ns        69509

benchmark_divide_units_simd<int8_t, int8_t, 32>                                   /256     25015 ns        25007 ns        28410
benchmark_divide_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>       /256     24891 ns        24884 ns        28246

benchmark_divide_units_simd<int64_t, int64_t, 32>                                 /256     24910 ns        24905 ns        28098
benchmark_divide_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>     /256     24888 ns        24881 ns        27959

benchmark_divide_units_simd<double, double, 32>                                   /256      4362 ns         4361 ns       160498
benchmark_divide_units_simd<double_wrapping_type, double_wrapping_type, 32>       /256      4371 ns         4370 ns       159631

benchmark_divide_units_simd<int64_t, double, 32>                                  /256      4363 ns         4362 ns       160427
benchmark_divide_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>      /256      4366 ns         4365 ns       160489

benchmark_divide_units_simd<double, int64_t, 32>                                  /256      4365 ns         4363 ns       160532
benchmark_divide_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>      /256      4372 ns         4371 ns       160567
```

# Licensing

See the [LICENSE](LICENSE) document.
