#include <gtest/gtest.h>
#include <stronk/can_pow.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_double_pow_test_type : stronk<a_double_pow_test_type, double, can_pow, can_equate_with_is_close>
{
    using stronk::stronk;
};

TEST(can_pow, can_pow_stronk_double)
{
    constexpr auto a = a_double_pow_test_type {2};

    EXPECT_EQ(a.pow(2), a_double_pow_test_type {4});
    EXPECT_EQ(a.pow(3), a_double_pow_test_type {8});
    EXPECT_EQ(a.pow(4), a_double_pow_test_type {16});
    EXPECT_EQ(a.pow(2.0), a_double_pow_test_type {4});
    EXPECT_EQ(a.pow(3.0), a_double_pow_test_type {8});
    EXPECT_EQ(a.pow(4.0), a_double_pow_test_type {16});
}

}  // namespace twig
