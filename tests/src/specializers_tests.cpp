#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>
#include <stronk/can_stream.h>
#include <stronk/prefabs.h>
#include <stronk/specializers.h>
#include <stronk/stronk.h>
#include <stronk/unit.h>
#include <stronk/utilities/macros.h>

namespace twig::tests
{

struct specializer_type_a : stronk_default_unit<specializer_type_a, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};
struct specializer_type_b : stronk_default_unit<specializer_type_b, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};

struct specializer_type_c : stronk_default_unit<specializer_type_c, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};

struct specializer_type_d
    : stronk<specializer_type_d, int32_t, multiplied_unit<specializer_type_a, specializer_type_c>::skill>
{
    using stronk::stronk;
};

}  // namespace twig::tests

namespace twig
{
template<>
struct underlying_multiply_operation<tests::specializer_type_a, tests::specializer_type_b>
{
    using res_type = int64_t;

    STRONK_FORCEINLINE
    constexpr static auto multiply(const typename tests::specializer_type_a::underlying_type& v1,
                                   const typename tests::specializer_type_b::underlying_type& v2) noexcept -> res_type
    {
        return (static_cast<int64_t>(v1) * static_cast<int64_t>(v2)) + 1LL;
    }
};

template<>
struct underlying_divide_operation<tests::specializer_type_a, tests::specializer_type_b>
{
    using res_type = int64_t;

    STRONK_FORCEINLINE
    constexpr static auto divide(const typename tests::specializer_type_a::underlying_type& v1,
                                 const typename tests::specializer_type_b::underlying_type& v2) noexcept -> res_type
    {
        return (static_cast<int64_t>(v1) / static_cast<int64_t>(v2)) + 1LL;
    }
};

template<>
struct unit_lookup<multiplied_unit<tests::specializer_type_a, tests::specializer_type_c>::dimensions_t, int32_t>
{
    using type = tests::specializer_type_d;
};

STRONK_SPECIALIZE_MULTIPLY(tests::specializer_type_a, tests::specializer_type_a);
STRONK_SPECIALIZE_MULTIPLY(tests::specializer_type_a, tests::specializer_type_b, can_equate);
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, tests::specializer_type_b, can_equate, can_ostream);

using specializer_type_a_squared = decltype(tests::specializer_type_a {} * tests::specializer_type_a {});
using specializer_type_a_times_b = decltype(tests::specializer_type_a {} * tests::specializer_type_b {});
using specializer_type_a_divided_by_b = decltype(tests::specializer_type_a {} / tests::specializer_type_b {});

STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_squared, tests::specializer_type_a);  // a^3
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_squared, tests::specializer_type_b);  // a^2 * b
// STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_times_b, tests::specializer_type_a); // a^2 * b
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_times_b, tests::specializer_type_b);       // a * b^2
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_divided_by_b, tests::specializer_type_a);  // a^2 / b
// STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_divided_by_b, tests::specializer_type_b); // a

// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_squared, tests::specializer_type_a); // a
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_squared, tests::specializer_type_b);  // a^2 / b
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_times_b, tests::specializer_type_a); // b
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_times_b, tests::specializer_type_b);  // a
STRONK_SPECIALIZE_DIVIDE(specializer_type_a_divided_by_b, tests::specializer_type_a);  // 1 / b
STRONK_SPECIALIZE_DIVIDE(specializer_type_a_divided_by_b, tests::specializer_type_b);  // a / b^2

// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_squared); // a
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_squared);  // b / a^2
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_times_b); // b
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_times_b); // a
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_divided_by_b);  // b
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_divided_by_b);  // b^2

}  // namespace twig

namespace twig::tests
{

TEST(underlying_multiply_operation, the_multiplying_function_is_overloaded)  // NOLINT
{
    using res_type = decltype(tests::specializer_type_a {} * tests::specializer_type_b {});

    // we have specialized it to return int64_t and add one to the result
    static_assert(std::is_same_v<res_type::underlying_type, int64_t>);
    EXPECT_EQ(tests::specializer_type_a {10} * tests::specializer_type_b {20}, res_type {200 + 1});
}

TEST(underlying_divide_operation, the_divide_function_is_overloaded)  // NOLINT
{
    using res_type = decltype(tests::specializer_type_a {} / tests::specializer_type_b {});

    // we have specialized it to return int64_t and add one to the result
    static_assert(std::is_same_v<res_type::underlying_type, int64_t>);
    EXPECT_EQ(tests::specializer_type_a {120} / tests::specializer_type_b {2}, res_type {60 + 1});
}

// clang-format off
static_assert(std::is_same_v<specializer_type_d, decltype(specializer_type_a {} * specializer_type_c {})>);
static_assert(std::is_same_v<unit_lookup<specializer_type_a_divided_by_b::dimensions_t, int64_t>::type, specializer_type_a_divided_by_b>);
static_assert(std::is_same_v<unit_lookup<specializer_type_a_times_b::dimensions_t, int64_t>::type, specializer_type_a_times_b>);
static_assert(std::is_same_v<unit_lookup<multiplied_unit<specializer_type_a, specializer_type_c>::dimensions_t, int32_t>::type, specializer_type_d>);
static_assert(std::is_same_v<unit_lookup<specializer_type_d::dimensions_t, int32_t>::type, specializer_type_d>);
// clang-format on

}  // namespace twig::tests
