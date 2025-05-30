#include <concepts>
#include <cstdint>
#include <ratio>

#include "stronk/unit.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "stronk/stronk.hpp"
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ >= 12)
#    include "stronk/extensions/fmt.h"  // IWYU pragma: keep
#endif
#include "stronk/extensions/gtest.h"  // IWYU pragma: keep for printing values in assertions
#include "stronk/utilities/dimensions.hpp"

namespace twig
{

// Example code:

// First we define our stronk types:
struct meters : unit<meters, std::ratio<1>, can_equate_underlying_type_specific, can_add>
{
};

struct seconds : unit<seconds, std::ratio<1>, can_equate_underlying_type_specific>
{
};

// ISO defines kg as the base scale of mass
struct kilograms : unit<kilograms, std::ratio<1>, can_equate>
{
};

using per_meter = divided_unit_t<identity_unit, meters>;
using square_meters = multiplied_unit_t<meters, meters>;
using meters_per_second = divided_unit_t<meters, seconds>;
using seconds_per_meter = divided_unit_t<seconds, meters>;
using acceleration_m_per_s2 = divided_unit_t<meters_per_second, seconds>;
using seconds_sq = multiplied_unit_t<seconds, seconds>;
using force = multiplied_unit_t<kilograms, acceleration_m_per_s2>;

struct a_regular_type
{
};
struct a_regular_stronk_type : stronk<a_regular_stronk_type, int32_t>
{
};

// Testing the concepts
static_assert(unit_like<kilograms>);
static_assert(unit_like<meters_per_second>);
static_assert(unit_like<acceleration_m_per_s2>);
static_assert(!unit_like<a_regular_stronk_type>);
static_assert(!unit_like<a_regular_type>);
static_assert(!unit_like<unit_value_t<meters_per_second, int32_t>>);

static_assert(unit_value_like<unit_value_t<kilograms, int32_t>>);
static_assert(unit_value_like<unit_value_t<meters_per_second, int32_t>>);
static_assert(unit_value_like<unit_value_t<acceleration_m_per_s2, int32_t>>);
static_assert(!unit_value_like<a_regular_stronk_type>);
static_assert(!unit_value_like<a_regular_type>);

// Testing the generated types

// clang-format off
using example_1 = stronk_default_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>, dimension<kilograms, -1>>, std::ratio<1>>;
using example_2 = stronk_default_unit<create_dimensions_t<dimension<kilograms, 1>, dimension<meters, 1>, dimension<seconds, -1>>, std::ratio<1>>;
static_assert(kilograms::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(!example_1::dimensions_t::is_pure());
static_assert(std::same_as<kilograms::dimensions_t::first_t::unit_t, kilograms>);
static_assert(std::same_as<multiplied_unit_t<meters, seconds>, stronk_default_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 1>>, std::ratio<1>>>);
static_assert(std::same_as<multiplied_unit_t<multiplied_unit_t<meters, seconds>, meters>, stronk_default_unit<create_dimensions_t<dimension<meters, 2>, dimension<seconds, 1>>,std::ratio<1>>>);
static_assert(std::same_as<divided_unit_t<multiplied_unit_t<meters, seconds>, meters>, seconds>);
static_assert(std::same_as<divided_unit_t<example_1, example_1>, identity_unit>);
static_assert(std::same_as<multiplied_unit_t<meters_per_second, seconds_per_meter>, identity_unit>);
static_assert(std::same_as<multiplied_unit_t<example_1, example_1>, stronk_default_unit<create_dimensions_t<dimension<meters, 4>, dimension<seconds, 2>, dimension<kilograms, -2>>,std::ratio<1>>>);
static_assert(std::same_as<multiplied_unit_t<example_1, example_2>, stronk_default_unit<create_dimensions_t<dimension<meters, 3>>,std::ratio<1>>>);
static_assert(std::same_as<multiplied_unit_t<example_2, example_1>, stronk_default_unit<create_dimensions_t<dimension<meters, 3>>,std::ratio<1>>>);
static_assert(std::same_as<divided_unit_t<example_1, example_2>, stronk_default_unit<create_dimensions_t<dimension<meters, 1>, dimension<seconds, 2>, dimension<kilograms, -2>>,std::ratio<1>>>);
static_assert(std::same_as<force, stronk_default_unit<create_dimensions_t<dimension<kilograms, 1>, dimension<meters, 1>, dimension<seconds, -2>>,std::ratio<1>>>);
// clang-format on

TEST(stronk_units_v2, when_multiplied_with_a_scalar_the_type_does_not_change_and_it_behaves_as_normally)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            using kg_int = unit_value_t<kilograms, int>;
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
    m_int_converted = m_double.cast<int>();
    EXPECT_EQ(m_int, m_int_converted);
}

struct using_default_unit : stronk_default_unit<using_default_unit, std::ratio<1>>
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

struct A : unit<A, std::ratio<1>, can_equate>
{
};
struct B : unit<B, std::ratio<1>, can_equate>
{
};
struct C : unit<multiplied_dimensions_t<A, B>, std::ratio<1>, can_equate>
{
};

struct D : unit<divided_dimensions_t<A, B>, std::ratio<1>, can_equate>
{
};

template<>
struct unit_lookup<multiplied_dimensions_t<A, B>>
{
    template<typename ScaleT>
    using unit_t = C::scaled_t<ScaleT>;
};

template<>
struct unit_lookup<divided_dimensions_t<A, B>>
{
    template<typename ScaleT>
    using unit_t = D::scaled_t<ScaleT>;
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

TEST(stronk_units_v2, scales_creates_the_right_types)
{
    auto m = make<meters, double>(1'000.0);
    auto km = make<std::kilo, meters, double>(4.0);
    auto um = make<std::micro, meters, double>(1'000.0);

    static_assert(std::same_as<decltype(m), meters::value<double>>);
    static_assert(std::same_as<decltype(km), meters::scaled_t<std::kilo>::value<double>>);
    static_assert(std::same_as<decltype(um), meters::scaled_t<std::micro>::value<double>>);

    static_assert(std::same_as<decltype(m * m), square_meters::value<double>>);
    static_assert(std::same_as<decltype(m * km), square_meters::scaled_t<std::kilo>::value<double>>);
    static_assert(std::same_as<decltype(m * um), square_meters::scaled_t<std::micro>::value<double>>);

    static_assert(std::same_as<decltype(km * km), square_meters::scaled_t<std::mega>::value<double>>);
    static_assert(std::same_as<decltype(km * um), square_meters::scaled_t<std::milli>::value<double>>);

    static_assert(std::same_as<decltype(um * um), square_meters::scaled_t<std::pico>::value<double>>);

    static_assert(std::same_as<decltype(m / m), double>);
    static_assert(std::same_as<decltype(m / km), identity_unit::scaled_t<std::ratio<1, 1000>>::value<double>>);
    static_assert(std::same_as<decltype(m / um), identity_unit::scaled_t<std::ratio<1'000'000, 1>>::value<double>>);
    static_assert(!std::same_as<decltype(m / km), double>);
    static_assert(!std::same_as<decltype(m / um), double>);

    auto m_2 = m * m;
    [[maybe_unused]]
    auto km_2 = km * km;
    [[maybe_unused]]
    auto um_2 = um * um;

    static_assert(std::same_as<decltype(m_2), square_meters::value<double>>);
    static_assert(std::same_as<decltype(km_2), square_meters::scaled_t<std::mega>::value<double>>);
    static_assert(std::same_as<decltype(um_2), square_meters::scaled_t<std::pico>::value<double>>);

    static_assert(std::same_as<decltype(m_2 / m), meters::value<double>>);
    static_assert(std::same_as<decltype(m_2 / km), meters::scaled_t<std::milli>::value<double>>);
    static_assert(std::same_as<decltype(m_2 / um), meters::scaled_t<std::mega>::value<double>>);

    static_assert(std::same_as<decltype(km_2 / m), meters::scaled_t<std::mega>::value<double>>);
    static_assert(std::same_as<decltype(km_2 / km), meters::scaled_t<std::kilo>::value<double>>);
    static_assert(std::same_as<decltype(km_2 / um), meters::scaled_t<std::tera>::value<double>>);

    static_assert(std::same_as<decltype(um_2 / m), meters::scaled_t<std::pico>::value<double>>);
    static_assert(std::same_as<decltype(um_2 / km), meters::scaled_t<std::femto>::value<double>>);
    static_assert(std::same_as<decltype(um_2 / um), meters::scaled_t<std::micro>::value<double>>);

    // now divide by squared
    static_assert(std::same_as<decltype(m / m_2), per_meter::value<double>>);
    static_assert(std::same_as<decltype(km / m_2), per_meter::scaled_t<std::kilo>::value<double>>);
    static_assert(std::same_as<decltype(um / m_2), per_meter::scaled_t<std::micro>::value<double>>);

    static_assert(std::same_as<decltype(m / km_2), per_meter::scaled_t<std::micro>::value<double>>);
    static_assert(std::same_as<decltype(km / km_2), per_meter::scaled_t<std::milli>::value<double>>);
    static_assert(std::same_as<decltype(um / km_2), per_meter::scaled_t<std::pico>::value<double>>);

    static_assert(std::same_as<decltype(m / um_2), per_meter::scaled_t<std::tera>::value<double>>);
    static_assert(std::same_as<decltype(km / um_2), per_meter::scaled_t<std::peta>::value<double>>);
    static_assert(std::same_as<decltype(um / um_2), per_meter::scaled_t<std::mega>::value<double>>);

    auto s = make<seconds, double>(1.0);
    auto hr = make<std::ratio<60UL * 60, 1>, seconds, double>(1.0);
    auto ms = make<std::milli, seconds, double>(1.0);
    auto d = make<std::ratio<60UL * 60 * 24, 1>, seconds, double>(1.0);

    static_assert(std::same_as<decltype(m / s), meters_per_second::value<double>>);
    static_assert(std::same_as<decltype(m / hr), meters_per_second::scaled_t<std::ratio<1, 3600>>::value<double>>);
    static_assert(std::same_as<decltype(km / hr), meters_per_second::scaled_t<std::ratio<5, 18>>::value<double>>);
    static_assert(std::same_as<decltype(km / d), meters_per_second::scaled_t<std::ratio<5, 432>>::value<double>>);
    static_assert(std::same_as<decltype(um / ms), meters_per_second::scaled_t<std::ratio<1, 1000>>::value<double>>);
    static_assert(std::same_as<decltype(ms / um), seconds_per_meter::scaled_t<std::ratio<1000, 1>>::value<double>>);

    [[maybe_unused]]
    auto per_m = 1 / m;
    [[maybe_unused]]
    auto per_km = 1 / km;
    [[maybe_unused]]
    auto per_um = 1 / um;

    static_assert(std::same_as<decltype(per_m), per_meter::value<double>>);
    static_assert(std::same_as<decltype(per_km), per_meter::scaled_t<std::ratio<1, 1'000>>::value<double>>);
    static_assert(std::same_as<decltype(per_um), per_meter::scaled_t<std::ratio<1'000'000, 1>>::value<double>>);

    static_assert(std::same_as<decltype(m * per_m), double>);
    static_assert(std::same_as<decltype(km * per_km), double>);
    static_assert(std::same_as<decltype(um * per_um), double>);
}

TEST(stronk_units_v2, scales_are_applied_correctly_when_converted)
{
    auto m = make<meters, double>(2.0);
    using km_t = meters::scaled_t<std::kilo>;
    using um_t = meters::scaled_t<std::micro>;

    is_unit<meters> auto km = m.to<km_t::value<double>>();
    auto expected_km = make<km_t>(2.0 / 1'000.0);
    EXPECT_EQ(km, expected_km);

    auto um = m.to<um_t::value<double>>();
    auto expected_um = make<um_t>(2'000'000.0);
    EXPECT_EQ(um, expected_um);

    um = make<std::kilo, meters>(3.0).to<um_t::value<double>>();
    expected_um = make<um_t>(3'000'000'000.0);
    EXPECT_EQ(um, expected_um);

    km = make<um_t>(4.0).to<km_t::value<double>>();
    expected_km = make<km_t>(4.0 / 1'000'000'000.0);
    EXPECT_EQ(km, expected_km);

    km = make<km_t>(8.0);
    auto s = make<seconds>(7200.0);
    auto km_per_s = km / s;
    auto expected_speed = unit_scaled_value_t<std::kilo, meters_per_second, double>(0.00111111111111);

    EXPECT_EQ(km_per_s, expected_speed);

    using hr_t = seconds::scaled_t<std::ratio<60UL * 60>>;
    using km_per_hr_t = meters_per_second::scaled_t<std::ratio<1000, 60UL * 60>>;

    auto hr = s.to<hr_t::value<double>>();
    auto expected_hr = make<hr_t>(2.0);
    EXPECT_EQ(hr, expected_hr);

    auto km_per_hr = km / hr;
    auto expected_km_per_hr = unit_value_t<km_per_hr_t, double>(4.0);

    EXPECT_EQ(km_per_hr, expected_km_per_hr);
    auto converted_to_km_per_hr = km_per_s.to<km_per_hr_t::value<double>>();
    EXPECT_EQ(converted_to_km_per_hr, expected_km_per_hr);
}

}  // namespace twig
