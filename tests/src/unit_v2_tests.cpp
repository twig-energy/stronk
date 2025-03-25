#include <concepts>
#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>
#include <stronk/stronk.h>
#include <stronk/unit_v2.h>
#include <stronk/utilities/dimensions.h>

namespace twig::unit_v2::unit_tests
{

// Example code:

// First we define our stronk types:
struct meters : unit<meters, can_equate_with_is_close>
{
};

struct seconds : unit<seconds, can_equate>
{
};

struct kilogram : unit<kilogram, can_equate>
{
};

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using meters_per_second = divided_unit_t<meters, seconds>;
using acceleration_m_per_s2 = divided_unit_t<meters_per_second, seconds>;
using seconds_sq = multiplied_unit_t<seconds, seconds>;
using force = multiplied_unit_t<kilogram, acceleration_m_per_s2>;

struct a_regular_type
{
};
struct a_regular_stronk_type : stronk<a_regular_stronk_type, int32_t>
{
};

// Testing the concepts
static_assert(unit_like<kilogram>);
static_assert(unit_like<meters_per_second>);
static_assert(unit_like<acceleration_m_per_s2>);
static_assert(!unit_like<a_regular_stronk_type>);
static_assert(!unit_like<a_regular_type>);
static_assert(!unit_like<unit_value_t<meters_per_second, int32_t>>);

static_assert(unit_value_like<unit_value_t<kilogram, int32_t>>);
static_assert(unit_value_like<unit_value_t<meters_per_second, int32_t>>);
static_assert(unit_value_like<unit_value_t<acceleration_m_per_s2, int32_t>>);
static_assert(!unit_value_like<a_regular_stronk_type>);
static_assert(!unit_value_like<a_regular_type>);

// Testing the generated types

// clang-format off
using example_1 = new_stronk_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>, dimension<kilogram, -1>>>;
using example_2 = new_stronk_unit<create_dimensions_t<dimension<kilogram, 1>, dimension<meters, 1>, dimension<seconds, -1>>>;
static_assert(kilogram::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::is_same_v<kilogram::dimensions_t::first_t::unit_t, kilogram>);
static_assert(std::is_same_v<multiplied_unit_t<meters, seconds>, new_stronk_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 1>>>>);
static_assert(std::is_same_v<multiplied_unit_t<multiplied_unit_t<meters, seconds>, meters>, new_stronk_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>>>>);
static_assert(std::is_same_v<divided_unit_t<multiplied_unit_t<meters, seconds>, meters>, seconds>);
static_assert(std::is_same_v<divided_unit_t<example_1, example_1>, identity_unit>);
static_assert(std::is_same_v<multiplied_unit_t<example_1, example_1>, new_stronk_unit<create_dimensions_t<dimension<meters, 4>, dimension<seconds, 2>, dimension<kilogram, -2>>>>);
static_assert(std::is_same_v<multiplied_unit_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<meters, 3>>>>);
static_assert(std::is_same_v<multiplied_unit_t<example_2, example_1>, new_stronk_unit<create_dimensions_t<dimension<meters, 3>>>>);
static_assert(std::is_same_v<divided_unit_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 2>, dimension<kilogram, -2>>>>);
// clang-format on

TEST(stronk_units_v2, when_multiplied_with_a_scalar_the_type_does_not_change_and_it_behaves_as_normally)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            using kg_int = unit_value_t<kilogram, int>;
            EXPECT_EQ(kg_int(i * j), kg_int(i) * j);
            EXPECT_EQ(kg_int(i * j), j * kg_int(i));
        }
    }
}

// NOLINTNEXTLINE
TEST(stronk_units_v2,
     when_multiplied_with_own_type_the_value_in_the_resulting_type_behaves_as_if_it_the_values_were_multiplied)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            using seconds_sq_int = unit_value_t<seconds_sq, int>;
            using seconds_int = unit_value_t<seconds, int>;
            EXPECT_EQ(seconds_sq_int {i * j}, seconds_int {i} * seconds_int {j});
        }
    }
}

TEST(stronk_units_v2, when_dividing_out_a_type_the_result_corrosponds_to_dividing_out_that_factor)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            using seconds_sq_int = unit_value_t<seconds_sq, int>;
            using seconds_int = unit_value_t<seconds, int>;
            if (j != 0) {
                EXPECT_EQ(seconds_int {i}, seconds_sq_int {i * j} / seconds_int {j});
            }
            if (i != 0) {
                EXPECT_EQ(seconds_int {j}, seconds_sq_int {i * j} / seconds_int {i});
            }
        }
    }
}

TEST(stronk_units_v2, one_over_unit_times_unit_is_one)
{
    for (auto i = -16; i < 16; i++) {
        if (i == 0) {
            continue;
        }
        using meters_double = unit_value_t<meters, double>;
        EXPECT_DOUBLE_EQ(1.0, (1.0 / meters_double {static_cast<double>(i)}) * meters_double(static_cast<double>(i)));
        EXPECT_DOUBLE_EQ(2.0, (2.0 / meters_double {static_cast<double>(i)}) * meters_double(static_cast<double>(i)));
    }
}

TEST(stronk_units_v2, when_dividing_a_unit_by_another_the_result_is_a_new_type_with_the_divided_values)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            using meters_per_second_double = unit_value_t<meters_per_second, double>;
            using meters_double = unit_value_t<meters, double>;
            using seconds_double = unit_value_t<seconds, double>;
            EXPECT_EQ(meters_per_second_double(static_cast<double>(i) / j),
                      meters_double(static_cast<double>(i)) / seconds_double {j});
        }
    }
}

TEST(stronk_units_v2, when_dividing_a_unit_by_a_squared_unit_the_result_is_a_new_type_with_the_divided_values)
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

                using acceleration_m_per_s2_double = unit_value_t<acceleration_m_per_s2, double>;
                using meters_double = unit_value_t<meters, double>;
                unit_value_t<seconds_sq, int> time_squared =
                    unit_value_t<seconds, int> {j} * unit_value_t<seconds, int> {k};
                EXPECT_EQ(acceleration_m_per_s2_double {static_cast<double>(i) / (j * k)},
                          meters_double(static_cast<double>(i)) / time_squared);
            }
        }
    }
}

TEST(stronk_units_v2, generated_units_can_add_and_subtract_and_compare_like_basic_types)
{
    for (auto i = -16; i < 16; i++) {
        using meters_per_second_double = unit_value_t<meters_per_second, double>;
        EXPECT_EQ(-meters_per_second_double {static_cast<double>(i)},
                  meters_per_second_double {static_cast<double>(-i)});
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(
                meters_per_second_double {static_cast<double>(i)} + meters_per_second_double {static_cast<double>(j)},
                meters_per_second_double {static_cast<double>(i + j)});
            EXPECT_EQ(
                meters_per_second_double {static_cast<double>(i)} - meters_per_second_double {static_cast<double>(j)},
                meters_per_second_double {static_cast<double>(i - j)});
            EXPECT_EQ(
                meters_per_second_double {static_cast<double>(i)} <=> meters_per_second_double {static_cast<double>(j)},
                i <=> j);
        }
    }
}

TEST(stronk_units_v2, make_function_can_create_units_of_different_types)
{
    auto m_int = make<meters>(1);
    auto m_double = make<meters>(1.0);
    static_assert(!std::same_as<decltype(m_int), decltype(m_double)>);
}

}  // namespace twig::unit_v2::unit_tests
