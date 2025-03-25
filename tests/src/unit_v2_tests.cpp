#include <cstdint>
#include <ratio>
#include <type_traits>

#include <gtest/gtest.h>
#include <stronk/stronk.h>
#include <stronk/unit_v2.h>
#include <stronk/utilities/dimensions.h>

namespace twig::unit_v2::unit_tests
{

// Example code:

// First we define our stronk types:
struct Distance : unit<Distance>
{
    template<typename T>
    struct value
        : stronk<value<T>, T, can_equate_with_is_close>
        , value_of_unit<Distance>
    {
        using stronk_base_t = ::twig::stronk<value<T>, T, can_equate_with_is_close>;
        using stronk_base_t::stronk_base_t;
    };
};
template<typename T>
using distance_t = Distance::template value<T>;

struct Time : unit<Time>
{
    template<typename T>
    struct value
        : stronk<value<T>, T, can_equate>
        , value_of_unit<Time>
    {
        using stronk_base_t = ::twig::stronk<value<T>, T, can_equate>;
        using stronk_base_t::stronk_base_t;
    };
    using unit_t = Time;
};
template<typename T>
using time_t = Time::template value<T>;

struct Mass : unit<Mass>
{
    template<typename T>
    struct value
        : stronk<value<T>, T, unit, can_equate>
        , value_of_unit<Mass>
    {
        using stronk_base_t = stronk<value<T>, T, unit, can_equate>;
        using stronk_base_t::stronk_base_t;
    };
};
template<typename T>
using mass_t = Mass::template value<T>;

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using Speed = divided_unit_t<Distance, Time>;
using Acceleration = divided_unit_t<Speed, Time>;
using TimeSquared = multiplied_unit_t<Time, Time>;
using Force = multiplied_unit_t<Mass, Acceleration>;

struct a_regular_type
{
};
struct a_regular_stronk_type : stronk<a_regular_stronk_type, int32_t>
{
};

// Testing the concepts
static_assert(unit_value_like<mass_t<int32_t>>);
static_assert(unit_value_like<Speed::value<int32_t>>);
static_assert(unit_value_like<Acceleration::value<int32_t>>);
static_assert(!unit_value_like<a_regular_stronk_type>);
static_assert(!unit_value_like<a_regular_type>);

// Testing the generated types

// clang-format off
using example_1 = new_stronk_unit<create_dimensions_t<dimension<Distance, 2>, dimension<Time, 1>, dimension<Mass, -1>>>;
using example_2 = new_stronk_unit<create_dimensions_t<dimension<Mass, 1>, dimension<Distance, 1>, dimension<Time, -1>>>;
static_assert(Mass::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::is_same_v<Mass::dimensions_t::first_t::unit_t, Mass>);
static_assert(std::is_same_v<multiplied_unit_t<Distance, Time>, new_stronk_unit<create_dimensions_t<dimension<Distance, 1>, dimension<Time, 1>>>>);
static_assert(std::is_same_v<multiplied_unit_t<multiplied_unit_t<Distance, Time>, Distance>, new_stronk_unit<create_dimensions_t<dimension<Distance, 2>, dimension<Time, 1>>>>);
static_assert(std::is_same_v<divided_unit_t<multiplied_unit_t<Distance, Time>, Distance>, Time>);
static_assert(std::is_same_v<divided_unit_t<example_1, example_1>, identity_unit>);
static_assert(std::is_same_v<multiplied_unit_t<example_1, example_1>, new_stronk_unit<create_dimensions_t<dimension<Distance, 4>, dimension<Time, 2>, dimension<Mass, -2>>>>);
static_assert(std::is_same_v<multiplied_unit_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<Distance, 3>>>>);
static_assert(std::is_same_v<multiplied_unit_t<example_2, example_1>, new_stronk_unit<create_dimensions_t<dimension<Distance, 3>>>>);
static_assert(std::is_same_v<divided_unit_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<Distance, 1>, dimension<Time, 2>, dimension<Mass, -2>>>>);
// clang-format on

TEST(stronk_units_v2, when_multiplied_with_a_scalar_the_type_does_not_change_and_it_behaves_as_normally)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(mass_t<int> {i * j},
                      mass_t<int> {i} * j);  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
            EXPECT_EQ(mass_t<int> {i * j},
                      j * mass_t<int> {i});  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
        }
    }
}

// NOLINTNEXTLINE
TEST(stronk_units_v2,
     when_multiplied_with_own_type_the_value_in_the_resulting_type_behaves_as_if_it_the_values_were_multiplied)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(TimeSquared::value<int> {i * j}, time_t<int> {i} * time_t<int> {j});  // NOLINT
        }
    }
}

TEST(stronk_units_v2, when_dividing_out_a_type_the_result_corrosponds_to_dividing_out_that_factor)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j != 0) {
                EXPECT_EQ(time_t<int> {i}, TimeSquared::value<int> {i * j} / time_t<int> {j});
            }
            if (i != 0) {
                EXPECT_EQ(time_t<int> {j}, TimeSquared::value<int> {i * j} / time_t<int> {i});
            }
        }
    }
}

TEST(stronk_units_v2, one_over_unit_times_unit_is_one)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        if (i == 0) {
            continue;
        }
        EXPECT_DOUBLE_EQ(
            1.0, (1.0 / distance_t<double> {static_cast<double>(i)}) * distance_t<double> {static_cast<double>(i)});
        EXPECT_DOUBLE_EQ(
            2.0, (2.0 / distance_t<double> {static_cast<double>(i)}) * distance_t<double> {static_cast<double>(i)});
    }
}

TEST(stronk_units_v2, when_dividing_a_unit_by_another_the_result_is_a_new_type_with_the_divided_values)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            EXPECT_EQ(Speed::value<double> {static_cast<double>(i) / j},
                      distance_t<double> {static_cast<double>(i)} / time_t<double> {j});
        }
    }
}

TEST(stronk_units_v2,
     when_dividing_a_unit_by_a_squared_unit_the_result_is_a_new_type_with_the_divided_values)  // NOLINT
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
                TimeSquared::value<int> time_squared = time_t<int> {j} * time_t<int> {k};
                EXPECT_EQ(Acceleration::value<double> {static_cast<double>(i) / (j * k)},
                          distance_t<double> {static_cast<double>(i)} / time_squared);
            }
        }
    }
}

TEST(stronk_units_v2, generated_units_can_add_and_subtract_and_compare_like_basic_types)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(-Speed::value<double> {static_cast<double>(i)}, Speed::value<double> {static_cast<double>(-i)});
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(Speed::value<double> {static_cast<double>(i)} + Speed::value<double> {static_cast<double>(j)},
                      Speed::value<double> {static_cast<double>(i + j)});
            EXPECT_EQ(Speed::value<double> {static_cast<double>(i)} - Speed::value<double> {static_cast<double>(j)},
                      Speed::value<double> {static_cast<double>(i - j)});
            EXPECT_EQ(Speed::value<double> {static_cast<double>(i)} <=> Speed::value<double> {static_cast<double>(j)},
                      i <=> j);
        }
    }
}

}  // namespace twig::unit_v2::unit_tests
