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

## An easy to customize, strong type library with built-in support for unit-like behavior

#### What:
- Easy support for stron{g|k} typing, with plenty of built-in skills to add functionality to your own types.
- Automatically combine types with physics-like unit behavior: `10 [Meter] / 2 [Second] = 5 [Meter / Second]`.

#### Why:
- Strong types allow you to catch argument ordering mismatches.
- Unit-like behavior allows you to use the type system to verify the correctness of your implementation.
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
struct LastName : twig::stronk<LastName, std::string>
{
    using stronk::stronk;
};

// Strong types protects you from accidentally passing the wrong argument to the wrong parameter.
void print_name(const LastName& lastname, const FirstName& firstname)
{
    // The twig::can_stream skill overloads the `operator<<(ostream&)` for your type.
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

    // Multiplying and dividing with an identity_unit (such as floats and integers) does not change the type.
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
    WattHours watt_hours = Hours {3.} * Watt {25.};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours -= WattHours {10.} + WattHours {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    Hours hours = watt_hours / Watt {25.};
    Watt watt = watt_hours / Hours {3.};
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
    // twig::make allows you to scale the input value but it does not change the resulting type
    WattHours one_mega_watt_hour = twig::make<std::mega, WattHours>(1.);
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_mega_watt_hour = Euro {300.} / one_mega_watt_hour;

    // This flexibility allows us to write expessive code, while having the type system check our implementation.
    Euro price_for_buying_5_mega_watt_hours = euros_per_mega_watt_hour * twig::make<std::mega, WattHours>(5.);

    auto mega_watt_hours_per_euro = 1. / euros_per_mega_watt_hour;  // `(Watt * Hours) / Euro`
    WattHours mega_watt_hours_affordable_for_500_euros = mega_watt_hours_per_euro * Euro {500.};
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
- `can_stream`: overloads `operator<<(std::ostream)`, stream the underlying value to the stream.
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

### Units
- `unit`: enables unit behavior for multiplication and division.
- `identity_unit`: enables unit behavior, but does not affect the type of multiplication and division.

### Third Party Library extensions (see `stronk/extensions/<library>.h`):
- `can_absl_hash`: implements the `AbslHashValue` friend function.
- `can_gtest_print`: for printing the values in gtest check macros
- `can_fmt_format`: implements `struct fmt::formatter<T>` with default formatting string `"{}"`. In the future we will add a `can_format` for `std::format`.
- `can_fmt_format_builder<"fmt format string{}">::skill`: implements `struct fmt::formatter<T>`. In the future we will add a `can_format_builder<"std format string">` for `std::format`.

Adding new skills is easy so feel free to add more.

## Prefabs: (see `stronk/prefabs.h`)
Often you might just need a group of skills for your specific types. For this you can use prefabs.

- `stronk_default_unit`: arithmetic unit with most of the regular operations
- `stronk_flag`: flag like boolean with equal operators etc.

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

In the extensions subfolder we have added skills for common third party libraries: `fmt`, `absl` and `gtest`. Using these also requires the relevant third party libraries to be installed.

# Building and installing

For more information on how to build see the [BUILDING](BUILDING.md) and [HACKING](HACKING.md) documents.

# Benchmarks
Stronk is a close to zero cost abstraction - performance varies per compiler and we get the best results when running with clang-13. You can see benchmark results for all the tested platforms in the [Continuous Integration Workflow](https://github.com/twig-energy/stronk/actions/workflows/ci.yml) workflow.

```
2022-09-06T14:43:44+02:00
Running ./build/dev/benchmarks/stronk_benchmarks
Run on (64 X 4549.12 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x32)
  L1 Instruction 32 KiB (x32)
  L2 Unified 512 KiB (x32)
  L3 Unified 16384 KiB (x8)
Load Average: 10.21, 7.37, 6.92
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-----------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                           Time             CPU   Iterations
-----------------------------------------------------------------------------------------------------------------------------------------------------
benchmark_default_onto_reserved_vector<int8_t>/32                                                                9.17 ns         9.17 ns     76320044
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>/32                                                  9.25 ns         9.24 ns     75346969
benchmark_default_onto_reserved_vector<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/32           9.45 ns         9.45 ns     75075028
benchmark_default_onto_reserved_vector<int8_t>/8192                                                              2206 ns         2205 ns       250185
benchmark_default_onto_reserved_vector<int8_t_wrapping_type>/8192                                                2156 ns         2155 ns       346698
benchmark_default_onto_reserved_vector<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/8192         2252 ns         2250 ns       343834

benchmark_default_onto_reserved_vector<int64_t>/32                                                               9.14 ns         9.14 ns     78489801
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>/32                                                 9.59 ns         9.59 ns     74623569
benchmark_default_onto_reserved_vector<int64_t>/8192                                                             2314 ns         2314 ns       335606
benchmark_default_onto_reserved_vector<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/32         9.24 ns         9.23 ns     76368670
benchmark_default_onto_reserved_vector<int64_t_wrapping_type>/8192                                               2157 ns         2156 ns       336977
benchmark_default_onto_reserved_vector<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/8192       2134 ns         2133 ns       335419

benchmark_default_onto_reserved_vector<std::string>/32                                                           71.7 ns         71.7 ns      9786965
benchmark_default_onto_reserved_vector<string_wrapping_type>/32                                                  87.6 ns         87.6 ns      8008157
benchmark_default_onto_reserved_vector<std::string>/8192                                                        18651 ns        18646 ns        37763
benchmark_default_onto_reserved_vector<string_wrapping_type>/8192                                               22081 ns        22076 ns        31908

benchmark_rand_onto_reserved_vector<int8_t>/32                                                                   8.73 ns         8.73 ns     79400682
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>/32                                                     8.89 ns         8.89 ns     78660701
benchmark_rand_onto_reserved_vector<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/32              9.58 ns         9.58 ns     73406516
benchmark_rand_onto_reserved_vector<int8_t>/8192                                                                 2019 ns         2019 ns       345569
benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>/8192                                                   2054 ns         2054 ns       342002
benchmark_rand_onto_reserved_vector<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/8192            2020 ns         2019 ns       342497

benchmark_rand_onto_reserved_vector<int64_t>/32                                                                  16.8 ns         16.8 ns     41869417
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>/32                                                    9.08 ns         9.08 ns     73551517
benchmark_rand_onto_reserved_vector<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/32            9.36 ns         9.36 ns     61747996
benchmark_rand_onto_reserved_vector<int64_t>/8192                                                                4262 ns         4261 ns       170606
benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>/8192                                                  2119 ns         2119 ns       333995
benchmark_rand_onto_reserved_vector<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/8192          2067 ns         2066 ns       344126

benchmark_rand_onto_reserved_vector<std::string>/32                                                             18065 ns        18062 ns        38505
benchmark_rand_onto_reserved_vector<string_wrapping_type>/32                                                    18393 ns        18387 ns        37943
benchmark_rand_onto_reserved_vector<std::string>/8192                                                         4695393 ns      4694391 ns          148
benchmark_rand_onto_reserved_vector<string_wrapping_type>/8192                                                4722350 ns      4721128 ns          149

benchmark_copy_vector_of<int8_t>/32                                                                              11.6 ns         11.6 ns     60235505
benchmark_copy_vector_of<int8_t_wrapping_type>/32                                                                11.7 ns         11.7 ns     59441445
benchmark_copy_vector_of<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/32                         12.0 ns         12.0 ns     58197395
benchmark_copy_vector_of<int8_t>/8192                                                                             101 ns          100 ns      6961617
benchmark_copy_vector_of<int8_t_wrapping_type>/8192                                                               102 ns          102 ns      6910298
benchmark_copy_vector_of<decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {})>/8192                        559 ns          559 ns      1252924

benchmark_copy_vector_of<int64_t>/32                                                                             12.0 ns         11.9 ns     57299235
benchmark_copy_vector_of<int64_t_wrapping_type>/32                                                               11.8 ns         11.8 ns     58296670
benchmark_copy_vector_of<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/32                       11.8 ns         11.8 ns     59240101
benchmark_copy_vector_of<int64_t>/8192                                                                           1076 ns         1076 ns       651109
benchmark_copy_vector_of<int64_t_wrapping_type>/8192                                                             1081 ns         1081 ns       646208
benchmark_copy_vector_of<decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {})>/8192                     1078 ns         1078 ns       649862

benchmark_copy_vector_of<std::string>/32                                                                          518 ns          518 ns      1347066
benchmark_copy_vector_of<string_wrapping_type>/32                                                                 527 ns          526 ns      1326543
benchmark_copy_vector_of<std::string>/8192                                                                     231834 ns       231781 ns         3035
benchmark_copy_vector_of<string_wrapping_type>/8192                                                            220968 ns       220921 ns         3165

benchmark_add_units<int8_t>/8192                                                                                 4035 ns         4034 ns       173803
benchmark_add_units<int8_t_wrapping_type>/8192                                                                   4027 ns         4026 ns       173742
benchmark_add_units<int64_t>/8192                                                                                4021 ns         4020 ns       174715
benchmark_add_units<int64_t_wrapping_type>/8192                                                                  4034 ns         4033 ns       173474
benchmark_add_units<double>/8192                                                                                 4036 ns         4035 ns       173319
benchmark_add_units<double_wrapping_type>/8192                                                                   4027 ns         4026 ns       173508

benchmark_add_units_simd<int8_t, 32>/256                                                                          278 ns          278 ns      2530135
benchmark_add_units_simd<int8_t_wrapping_type, 32>/256                                                            276 ns          276 ns      2525227
benchmark_add_units_simd<int64_t, 32>/256                                                                        1791 ns         1791 ns       391018
benchmark_add_units_simd<int64_t_wrapping_type, 32>/256                                                          1792 ns         1791 ns       390175
benchmark_add_units_simd<double, 32>/256                                                                         1794 ns         1794 ns       391760
benchmark_add_units_simd<double_wrapping_type, 32>/256                                                           1820 ns         1820 ns       387786

benchmark_subtract_units<int8_t>/8192                                                                            4063 ns         4062 ns       172906
benchmark_subtract_units<int8_t_wrapping_type>/8192                                                              4073 ns         4072 ns       172257
benchmark_subtract_units<int64_t>/8192                                                                           4045 ns         4044 ns       172458
benchmark_subtract_units<int64_t_wrapping_type>/8192                                                             4028 ns         4027 ns       173719
benchmark_subtract_units<double>/8192                                                                            4028 ns         4027 ns       173679
benchmark_subtract_units<double_wrapping_type>/8192                                                              4017 ns         4016 ns       174556

benchmark_subtract_units_simd<int8_t, 32>/256                                                                     275 ns          274 ns      2552599
benchmark_subtract_units_simd<int8_t_wrapping_type, 32>/256                                                       277 ns          277 ns      2545373
benchmark_subtract_units_simd<int64_t, 32>/256                                                                   1779 ns         1779 ns       393392
benchmark_subtract_units_simd<int64_t_wrapping_type, 32>/256                                                     1812 ns         1811 ns       387452
benchmark_subtract_units_simd<double, 32>/256                                                                    1787 ns         1787 ns       390242
benchmark_subtract_units_simd<double_wrapping_type, 32>/256                                                      1784 ns         1784 ns       391717

benchmark_multiply_units<int8_t, int8_t>/8192                                                                    4802 ns         4801 ns       145996
benchmark_multiply_units<int8_t_wrapping_type, int8_t_wrapping_type>/8192                                        4903 ns         4902 ns       117718
benchmark_multiply_units<int64_t, int64_t>/8192                                                                  4121 ns         4120 ns       169752
benchmark_multiply_units<int64_t_wrapping_type, int64_t_wrapping_type>/8192                                      4123 ns         4122 ns       169868
benchmark_multiply_units<double, double>/8192                                                                    4021 ns         4021 ns       174573
benchmark_multiply_units<double_wrapping_type, double_wrapping_type>/8192                                        4021 ns         4020 ns       174052
benchmark_multiply_units<int64_t, double>/8192                                                                   4037 ns         4036 ns       173381
benchmark_multiply_units<int64_t_wrapping_type, double_wrapping_type>/8192                                       4023 ns         4021 ns       174258
benchmark_multiply_units<double, int64_t>/8192                                                                   4022 ns         4021 ns       174436
benchmark_multiply_units<double_wrapping_type, int64_t_wrapping_type>/8192                                       4089 ns         4087 ns       173909

benchmark_multiply_units_simd<int8_t, int8_t, 32>/256                                                            2182 ns         2181 ns       323152
benchmark_multiply_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>/256                                2200 ns         2200 ns       318294
benchmark_multiply_units_simd<int64_t, int64_t, 32>/256                                                          2729 ns         2728 ns       258257
benchmark_multiply_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>/256                              2663 ns         2663 ns       260253
benchmark_multiply_units_simd<double, double, 32>/256                                                            1787 ns         1786 ns       389684
benchmark_multiply_units_simd<double_wrapping_type, double_wrapping_type, 32>/256                                1786 ns         1785 ns       391853
benchmark_multiply_units_simd<int64_t, double, 32>/256                                                           3820 ns         3819 ns       182211
benchmark_multiply_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>/256                               3818 ns         3817 ns       183600
benchmark_multiply_units_simd<double, int64_t, 32>/256                                                           3937 ns         3935 ns       177883
benchmark_multiply_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>/256                               3950 ns         3949 ns       176948

benchmark_divide_units<int8_t, int8_t>/8192                                                                     28074 ns        28068 ns        24830
benchmark_divide_units<int8_t_wrapping_type, int8_t_wrapping_type>/8192                                         28046 ns        28040 ns        24942
benchmark_divide_units<int64_t, int64_t>/8192                                                                   28056 ns        28050 ns        24932
benchmark_divide_units<int64_t_wrapping_type, int64_t_wrapping_type>/8192                                       28082 ns        28076 ns        24944
benchmark_divide_units<double, double>/8192                                                                     10033 ns        10030 ns        69832
benchmark_divide_units<double_wrapping_type, double_wrapping_type>/8192                                         10021 ns        10019 ns        69867
benchmark_divide_units<int64_t, double>/8192                                                                    10021 ns        10019 ns        69825
benchmark_divide_units<int64_t_wrapping_type, double_wrapping_type>/8192                                         9901 ns         9899 ns        69931
benchmark_divide_units<double, int64_t>/8192                                                                     9921 ns         9919 ns        71102
benchmark_divide_units<double_wrapping_type, int64_t_wrapping_type>/8192                                        10068 ns        10066 ns        70293

benchmark_divide_units_simd<int8_t, int8_t, 32>/256                                                             24108 ns        24099 ns        28758
benchmark_divide_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>/256                                 24060 ns        24054 ns        29132
benchmark_divide_units_simd<int64_t, int64_t, 32>/256                                                           24195 ns        24189 ns        28944
benchmark_divide_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>/256                               24277 ns        24272 ns        28865
benchmark_divide_units_simd<double, double, 32>/256                                                              4352 ns         4351 ns       161269
benchmark_divide_units_simd<double_wrapping_type, double_wrapping_type, 32>/256                                  4327 ns         4326 ns       161498
benchmark_divide_units_simd<int64_t, double, 32>/256                                                             4363 ns         4362 ns       160822
benchmark_divide_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>/256                                 4364 ns         4363 ns       160992
benchmark_divide_units_simd<double, int64_t, 32>/256                                                             4364 ns         4363 ns       157961
benchmark_divide_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>/256                                 4355 ns         4354 ns       159294
```

# Licensing

See the [LICENSE](LICENSE.md) document.
