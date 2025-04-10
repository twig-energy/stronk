#include <concepts>
#include <cstdint>

#include <fmt/format.h>
#include <gtest/gtest.h>
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ >= 12)
#    include <stronk/extensions/fmt.h>  // IWYU pragma: keep
#endif
#include <stronk/stronk.h>
#include <stronk/unit_v2.h>
#include <stronk/utilities/dimensions.h>

namespace twig::unit_v2
{

// Example code:

// First we define our stronk types:
struct meters : unit<meters, can_equate_underlying_type_specific, can_add>
{
};

struct seconds : unit<seconds, can_equate>
{
};

struct kilogram : unit<kilogram, can_equate>
{
};

// The name of the generated type for `meters` over `seconds` is not really reader-friendly so making an alias can be
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
using example_1 = stronk_default_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>, dimension<kilogram, -1>>>;
using example_2 = stronk_default_unit<create_dimensions_t<dimension<kilogram, 1>, dimension<meters, 1>, dimension<seconds, -1>>>;
static_assert(kilogram::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::same_as<kilogram::dimensions_t::first_t::unit_t, kilogram>);
static_assert(std::same_as<multiplied_unit_t<meters, seconds>, stronk_default_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 1>>>>);
static_assert(std::same_as<multiplied_unit_t<multiplied_unit_t<meters, seconds>, meters>, stronk_default_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>>>>);
static_assert(std::same_as<divided_unit_t<multiplied_unit_t<meters, seconds>, meters>, seconds>);
static_assert(std::same_as<divided_unit_t<example_1, example_1>, identity_unit>);
static_assert(std::same_as<multiplied_unit_t<example_1, example_1>, stronk_default_unit<create_dimensions_t<dimension<meters, 4>, dimension<seconds, 2>, dimension<kilogram, -2>>>>);
static_assert(std::same_as<multiplied_unit_t<example_1, example_2>, stronk_default_unit<create_dimensions_t<dimension<meters, 3>>>>);
static_assert(std::same_as<multiplied_unit_t<example_2, example_1>, stronk_default_unit<create_dimensions_t<dimension<meters, 3>>>>);
static_assert(std::same_as<divided_unit_t<example_1, example_2>, stronk_default_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 2>, dimension<kilogram, -2>>>>);
static_assert(std::same_as<force, stronk_default_unit<create_dimensions_t<dimension<kilogram, 1>, dimension<meters, 1>, dimension<seconds, -2>>>>);
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

TEST(stronk_units_v2, when_dividing_out_a_type_the_result_corresponds_to_dividing_out_that_factor)
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
            EXPECT_EQ(make<meters_per_second>(static_cast<double>(i) / j),
                      make<meters>(static_cast<double>(i)) / make<seconds>(j));
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
    auto m_int = make<meters>(1) + make<meters>(1);
    auto m_double = make<meters>(2.1);
    static_assert(!std::same_as<decltype(m_int), decltype(m_double)>);
    static_assert(std::same_as<decltype(m_int)::unit_t, decltype(m_double)::unit_t>);

    auto m_int_converted = static_cast<meters::value<int>>(m_double);
    static_assert(std::same_as<decltype(m_int), decltype(m_int_converted)>);

    EXPECT_EQ(m_int, m_int_converted);
}

struct using_default_unit : stronk_default_unit<using_default_unit>
{
};

TEST(stronk_units_v2, default_unit_can_do_all_the_usual_stuff)
{
    auto val = make<using_default_unit>(1);
    EXPECT_EQ(make<using_default_unit>(2), val * 2);
    EXPECT_EQ(make<using_default_unit>(2), val + val);
    EXPECT_EQ(make<using_default_unit>(2), val * 3 - val);
    EXPECT_EQ(make<using_default_unit>(2), -val * -2);
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ >= 12)
    EXPECT_EQ("1", fmt::to_string(val));
#endif
    EXPECT_LT(val, val * 2);
    EXPECT_GT(val, -val);
}

struct A : unit<A, can_equate>
{
};
struct B : unit<B, can_equate>
{
};
struct C : unit<multiplied_dimensions_t<A, B>, can_equate>
{
};

struct D : unit<divided_dimensions_t<A, B>, can_equate>
{
};

template<>
struct unit_lookup<multiplied_dimensions_t<A, B>>
{
    using unit_t = C;
};

template<>
struct unit_lookup<divided_dimensions_t<A, B>>
{
    using unit_t = D;
};

TEST(stronk_units_v2, can_override_units)
{
    auto a = make<A, int>(4);
    auto b = make<B, int>(2);
    auto c = a * b;

    static_assert(std::same_as<decltype(c), C::value<int>>);
    auto expected_c = make<C, int>(8);
    EXPECT_EQ(c, expected_c);

    auto back_to_a = c / b;
    EXPECT_EQ(back_to_a, a);
    auto back_to_b = c / a;
    EXPECT_EQ(back_to_b, b);

    auto d = a / b;
    static_assert(std::same_as<decltype(d), D::value<int>>);
    auto expected_d = make<D, int>(2);
    EXPECT_EQ(d, expected_d);

    back_to_a = d * b;
    EXPECT_EQ(back_to_a, a);
    back_to_b = a / d;
    EXPECT_EQ(back_to_b, b);
}

}  // namespace twig::unit_v2
