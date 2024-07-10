#include <cstdint>
#include <ratio>
#include <type_traits>

#include <gtest/gtest.h>
#include <stronk/extensions/gtest.h>
#include <stronk/prefabs.h>
#include <stronk/stronk.h>
#include <stronk/unit.h>

#include "stronk/utilities/constexpr_helpers.h"

namespace twig::unit_tests
{

// Example code:

// First we define our stronk types:
struct Distance : stronk<Distance, double, unit, can_equate_with_is_close, can_gtest_print>
{
    using stronk::stronk;
};

struct Time : stronk<Time, int64_t, unit, can_equate, can_gtest_print>
{
    using stronk::stronk;
};

struct Mass : stronk<Mass, int64_t, unit, can_equate, can_gtest_print>
{
    using stronk::stronk;
};

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using Speed = decltype(Distance {} / Time {});
using Acceleration = decltype(Speed {} / Time {});
using TimeSquared = decltype(Time {} * Time {});
using Force = decltype(Mass {} * Acceleration {});

struct a_regular_type
{};
struct a_regular_stronk_type : stronk<a_regular_stronk_type, int32_t>
{};

// Testing the concepts
static_assert(unit_like<Mass>);
static_assert(!identity_unit_like<Mass>);
static_assert(unit_like<Speed>);
static_assert(!identity_unit_like<Speed>);
static_assert(unit_like<Acceleration>);
static_assert(!identity_unit_like<Acceleration>);
static_assert(!unit_like<a_regular_stronk_type>);
static_assert(!identity_unit_like<a_regular_stronk_type>);
static_assert(!unit_like<a_regular_type>);
static_assert(!identity_unit_like<a_regular_type>);

// Testing the generated types

// clang-format off
static_assert(Mass::dimensions_t::is_pure());
using example_1 = NewStronkUnit<int32_t, create_dimensions_t<Dimension<Distance, 2>, Dimension<Time, 1>, Dimension<Mass, -1>>>;
using example_2 = NewStronkUnit<int32_t, create_dimensions_t<Dimension<Mass, 1>, Dimension<Distance, 1>, Dimension<Time, -1>>>;
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::is_same_v<Mass::dimensions_t::first_t::unit_t, Mass>);
static_assert(std::is_same_v<multiply_t<Distance, Time>, NewStronkUnit<double, create_dimensions_t<Dimension<Distance, 1>, Dimension<Time, 1>>>>);
static_assert(std::is_same_v<multiply_t<multiply_t<Distance, Time>, Distance>, NewStronkUnit<double, create_dimensions_t<Dimension<Distance, 2>, Dimension<Time, 1>>>>);
static_assert(std::is_same_v<divide_t<multiply_t<Distance, Time>, Distance>, Time>);
static_assert(std::is_same_v<divide_t<example_1, example_1>, int32_t>);
static_assert(std::is_same_v<multiply_t<example_1, example_1>, NewStronkUnit<int32_t, create_dimensions_t<Dimension<Distance, 4>, Dimension<Time, 2>, Dimension<Mass, -2>>>>);
static_assert(std::is_same_v<multiply_t<example_1, example_2>, NewStronkUnit<int32_t, create_dimensions_t<Dimension<Distance, 3>>>>);
static_assert(std::is_same_v<multiply_t<example_2, example_1>, NewStronkUnit<int32_t, create_dimensions_t<Dimension<Distance, 3>>>>);
static_assert(std::is_same_v<divide_t<example_1, example_2>, NewStronkUnit<int32_t, create_dimensions_t<Dimension<Distance, 1>, Dimension<Time, 2>, Dimension<Mass, -2>>>>);
// clang-format on

TEST(stronk_units, when_multiplied_with_a_scalar_the_type_does_not_change_and_it_behaves_as_normally)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(Mass {i * j}, Mass {i} * j);  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
            EXPECT_EQ(Mass {i * j}, j * Mass {i});  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
        }
    }
}

// NOLINTNEXTLINE
TEST(stronk_units,
     when_multiplied_with_own_type_the_value_in_the_resulting_type_behaves_as_if_it_the_values_were_multiplied)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(TimeSquared {i * j}, Time {i} * Time {j});  // NOLINT
        }
    }
}

TEST(stronk_units, when_dividing_out_a_type_the_result_corrosponds_to_dividing_out_that_factor)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j != 0) {
                EXPECT_EQ(Time {i}, TimeSquared {i * j} / Time {j});
            }
            if (i != 0) {
                EXPECT_EQ(Time {j}, TimeSquared {i * j} / Time {i});
            }
        }
    }
}

TEST(stronk_units, one_over_unit_times_unit_is_one)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        if (i == 0) {
            continue;
        }
        EXPECT_DOUBLE_EQ(1.0, (1.0 / Distance {static_cast<double>(i)}) * Distance {static_cast<double>(i)});
        EXPECT_DOUBLE_EQ(2.0, (2.0 / Distance {static_cast<double>(i)}) * Distance {static_cast<double>(i)});
    }
}

TEST(stronk_units, when_dividing_a_unit_by_another_the_result_is_a_new_type_with_the_divided_values)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            EXPECT_EQ(Speed {static_cast<double>(i) / j}, Distance {static_cast<double>(i)} / Time {j});
        }
    }
}

TEST(stronk_units, when_dividing_a_unit_by_a_squared_unit_the_result_is_a_new_type_with_the_divided_values)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            for (auto k = -16; k < 16; k++) {
                if (k == 0) {
                    continue;
                }
                TimeSquared time_squared = Time {j} * Time {k};
                EXPECT_EQ(Acceleration {static_cast<double>(i) / (j * k)},
                          Distance {static_cast<double>(i)} / time_squared);
            }
        }
    }
}

TEST(stronk_units, generated_units_can_add_and_subtract_and_compare_like_basic_types)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(-Speed {static_cast<double>(i)}, Speed {static_cast<double>(-i)});
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(Speed {static_cast<double>(i)} + Speed {static_cast<double>(j)},
                      Speed {static_cast<double>(i + j)});
            EXPECT_EQ(Speed {static_cast<double>(i)} - Speed {static_cast<double>(j)},
                      Speed {static_cast<double>(i - j)});
            EXPECT_EQ(Speed {static_cast<double>(i)} <=> Speed {static_cast<double>(j)}, i <=> j);
        }
    }
}

struct a_prefab_unit : stronk_default_unit<a_prefab_unit, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};

TEST(stronk_default_unit, prefab_units_can_add_and_subtract_and_compare_like_basic_types)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(-a_prefab_unit {i}, a_prefab_unit {-i});
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(a_prefab_unit {i} + a_prefab_unit {j}, a_prefab_unit {i + j});
            EXPECT_EQ(a_prefab_unit {i} - a_prefab_unit {j}, a_prefab_unit {i - j});
            EXPECT_EQ(a_prefab_unit {i} <=> a_prefab_unit {j}, i <=> j);
        }
    }
}

}  // namespace twig::unit_tests
