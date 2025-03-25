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
    struct Value : stronk<Value<T>, T, can_equate_with_is_close>
    {
        using stronk_base_t = ::twig::stronk<Value<T>, T, can_equate_with_is_close>;
        using stronk_base_t::stronk_base_t;
        using unit_t = Distance;
    };
};

struct Time : unit<Time>
{
    template<typename T>
    struct Value : stronk<Value<T>, T, can_equate>
    {
        using stronk_base_t = ::twig::stronk<Value<T>, T, can_equate>;
        using stronk_base_t::stronk_base_t;
        using unit_t = Time;
    };
    using unit_t = Time;
};

struct Mass : unit<Mass>
{
    template<typename T>
    struct Value : stronk<Value<T>, T, unit, can_equate>
    {
        using stronk_base_t = stronk<Value<T>, T, unit, can_equate>;
        using stronk_base_t::stronk_base_t;
        using unit_t = Mass;
    };
};

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using Speed = decltype(Distance::Value<double> {} / Time::Value<double> {})::unit_t;
using Acceleration = decltype(Speed::Value<double> {} / Time::Value<double> {})::unit_t;
using TimeSquared = decltype(Time::Value<double> {} * Time::Value<double> {})::unit_t;
using Force = decltype(Mass::Value<double> {} * Acceleration::Value<double> {})::unit_t;

struct a_regular_type
{
};
struct a_regular_stronk_type : stronk<a_regular_stronk_type, int32_t>
{
};

// Testing the concepts
static_assert(unit_like<Mass::Value<int32_t>>);
static_assert(!identity_unit_like<Mass>);
static_assert(unit_like<Speed::Value<int32_t>>);
static_assert(!identity_unit_like<Speed>);
static_assert(unit_like<Acceleration::Value<int32_t>>);
static_assert(!identity_unit_like<Acceleration>);
static_assert(!unit_like<a_regular_stronk_type>);
static_assert(!identity_unit_like<a_regular_stronk_type>);
static_assert(!unit_like<a_regular_type>);
static_assert(!identity_unit_like<a_regular_type>);

// Testing the generated types

// clang-format off
using example_1 = new_stronk_unit<create_dimensions_t<dimension<Distance, 2>, dimension<Time, 1>, dimension<Mass, -1>>>;
using example_2 = new_stronk_unit<create_dimensions_t<dimension<Mass, 1>, dimension<Distance, 1>, dimension<Time, -1>>>;
static_assert(Mass::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::is_same_v<Mass::dimensions_t::first_t::unit_t, Mass>);
static_assert(std::is_same_v<multiply_t<Distance, Time>, new_stronk_unit<create_dimensions_t<dimension<Distance, 1>, dimension<Time, 1>>>>);
static_assert(std::is_same_v<multiply_t<multiply_t<Distance, Time>, Distance>, new_stronk_unit<create_dimensions_t<dimension<Distance, 2>, dimension<Time, 1>>>>);
static_assert(std::is_same_v<divide_t<multiply_t<Distance, Time>, Distance>, Time>);
static_assert(std::is_same_v<divide_t<example_1, example_1>, identity_unit>);
static_assert(std::is_same_v<multiply_t<example_1, example_1>, new_stronk_unit<create_dimensions_t<dimension<Distance, 4>, dimension<Time, 2>, dimension<Mass, -2>>>>);
static_assert(std::is_same_v<multiply_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<Distance, 3>>>>);
static_assert(std::is_same_v<multiply_t<example_2, example_1>, new_stronk_unit<create_dimensions_t<dimension<Distance, 3>>>>);
static_assert(std::is_same_v<divide_t<example_1, example_2>, new_stronk_unit<create_dimensions_t<dimension<Distance, 1>, dimension<Time, 2>, dimension<Mass, -2>>>>);
// clang-format on

TEST(stronk_units, when_multiplied_with_a_scalar_the_type_does_not_change_and_it_behaves_as_normally)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(Mass::Value<int> {i * j},
                      Mass::Value<int> {i} * j);  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
            EXPECT_EQ(Mass::Value<int> {i * j},
                      j * Mass::Value<int> {i});  // NOLINT(bugprone-implicit-widening-of-multiplication-result)
        }
    }
}

// NOLINTNEXTLINE
TEST(stronk_units,
     when_multiplied_with_own_type_the_value_in_the_resulting_type_behaves_as_if_it_the_values_were_multiplied)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(TimeSquared::Value<int> {i * j}, Time::Value<int> {i} * Time::Value<int> {j});  // NOLINT
        }
    }
}

TEST(stronk_units, when_dividing_out_a_type_the_result_corrosponds_to_dividing_out_that_factor)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j != 0) {
                EXPECT_EQ(Time::Value<int> {i}, TimeSquared::Value<int> {i * j} / Time::Value<int> {j});
            }
            if (i != 0) {
                EXPECT_EQ(Time::Value<int> {j}, TimeSquared::Value<int> {i * j} / Time::Value<int> {i});
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
        EXPECT_DOUBLE_EQ(1.0,
                         (1.0 / Distance::Value<double> {static_cast<double>(i)})
                             * Distance::Value<double> {static_cast<double>(i)});
        EXPECT_DOUBLE_EQ(2.0,
                         (2.0 / Distance::Value<double> {static_cast<double>(i)})
                             * Distance::Value<double> {static_cast<double>(i)});
    }
}

TEST(stronk_units, when_dividing_a_unit_by_another_the_result_is_a_new_type_with_the_divided_values)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            EXPECT_EQ(Speed::Value<double> {static_cast<double>(i) / j},
                      Distance::Value<double> {static_cast<double>(i)} / Time::Value<double> {j});
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
                TimeSquared::Value<int> time_squared = Time::Value<int> {j} * Time::Value<int> {k};
                EXPECT_EQ(Acceleration::Value<double> {static_cast<double>(i) / (j * k)},
                          Distance::Value<double> {static_cast<double>(i)} / time_squared);
            }
        }
    }
}

TEST(stronk_units, generated_units_can_add_and_subtract_and_compare_like_basic_types)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(-Speed::Value<double> {static_cast<double>(i)}, Speed::Value<double> {static_cast<double>(-i)});
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(Speed::Value<double> {static_cast<double>(i)} + Speed::Value<double> {static_cast<double>(j)},
                      Speed::Value<double> {static_cast<double>(i + j)});
            EXPECT_EQ(Speed::Value<double> {static_cast<double>(i)} - Speed::Value<double> {static_cast<double>(j)},
                      Speed::Value<double> {static_cast<double>(i - j)});
            EXPECT_EQ(Speed::Value<double> {static_cast<double>(i)} <=> Speed::Value<double> {static_cast<double>(j)},
                      i <=> j);
        }
    }
}

}  // namespace twig::unit_v2::unit_tests
