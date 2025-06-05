#include <concepts>
#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>
#include <stronk/utilities/macros.hpp>

#include "stronk/unit.hpp"
#include "stronk/utilities/ratio.hpp"

namespace twig::tests
{

struct specializer_type_a : unit<specializer_type_a, twig::ratio<1>>
{
};
struct specializer_type_b : unit<specializer_type_b, twig::ratio<1>>
{
};

struct specializer_type_c : unit<specializer_type_c, twig::ratio<1>>
{
};

struct specializer_type_d : unit<multiplied_dimensions_t<specializer_type_a, specializer_type_c>, twig::ratio<1>>
{
};

}  // namespace twig::tests

namespace twig
{
template<>
struct underlying_multiply_operation<tests::specializer_type_a::value<int>, tests::specializer_type_b::value<int>>
{
    using res_type = int64_t;

    STRONK_FORCEINLINE
    constexpr static auto multiply(const typename tests::specializer_type_a::value<int>::underlying_type& v1,
                                   const typename tests::specializer_type_b::value<int>::underlying_type& v2) noexcept
        -> res_type
    {
        return (static_cast<int64_t>(v1) * static_cast<int64_t>(v2)) + 1LL;
    }
};

template<>
struct underlying_divide_operation<tests::specializer_type_a::value<int>, tests::specializer_type_b::value<int>>
{
    using res_type = int64_t;

    STRONK_FORCEINLINE
    constexpr static auto divide(const typename tests::specializer_type_a::value<int>::underlying_type& v1,
                                 const typename tests::specializer_type_b::value<int>::underlying_type& v2) noexcept
        -> res_type
    {
        return (static_cast<int64_t>(v1) / static_cast<int64_t>(v2)) + 1LL;
    }
};

template<>
struct unit_lookup<multiplied_dimensions_t<tests::specializer_type_a, tests::specializer_type_c>>
{
    template<scale_like ScaleT>
    using unit_t = unit_scaled_or_base_t<tests::specializer_type_d, ScaleT>;
};

using specializer_type_a_squared =
    decltype(tests::specializer_type_a::value<int> {} * tests::specializer_type_a::value<int> {});
using specializer_type_a_times_b =
    decltype(tests::specializer_type_a::value<int> {} * tests::specializer_type_b::value<int> {});
using specializer_type_a_divided_by_b =
    decltype(tests::specializer_type_a::value<int> {} / tests::specializer_type_b::value<int> {});

}  // namespace twig

namespace twig::tests
{

TEST(underlying_multiply_operation, the_multiplying_function_is_overloaded)  // NOLINT
{
    using res_type = decltype(tests::specializer_type_a::value<int> {} * tests::specializer_type_b::value<int> {});

    // we have specialized it to return int64_t and add one to the result
    static_assert(std::is_same_v<res_type::underlying_type, int64_t>);
    EXPECT_EQ(tests::specializer_type_a::value<int> {10} * tests::specializer_type_b::value<int> {20},
              res_type {200 + 1});
}

TEST(underlying_divide_operation, the_divide_function_is_overloaded)  // NOLINT
{
    using res_type = decltype(tests::specializer_type_a::value<int> {} / tests::specializer_type_b::value<int> {});

    // we have specialized it to return int64_t and add one to the result
    static_assert(std::is_same_v<res_type::underlying_type, int64_t>);
    EXPECT_EQ(tests::specializer_type_a::value<int> {120} / tests::specializer_type_b::value<int> {2},
              res_type {60 + 1});
}

// clang-format off

static_assert(std::same_as<specializer_type_d::value<int>, decltype(specializer_type_a::value<int> {} * specializer_type_c::value<int> {})>);
static_assert(std::same_as<unit_lookup<typename specializer_type_a_divided_by_b::unit_t::dimensions_t>::unit_t<twig::ratio<1>>::value<int64_t>, specializer_type_a_divided_by_b>);
static_assert(std::same_as<unit_lookup<typename specializer_type_a_times_b::unit_t::dimensions_t>::unit_t<twig::ratio<1>>::value<int64_t>, specializer_type_a_times_b>);
static_assert(std::same_as<unit_lookup<typename multiplied_unit_t<specializer_type_a, specializer_type_c>::dimensions_t>::unit_t<twig::ratio<1>>, specializer_type_d>);
static_assert(std::same_as<unit_lookup<typename specializer_type_d::dimensions_t>::unit_t<twig::ratio<1>>, specializer_type_d>);
// clang-format on

}  // namespace twig::tests
