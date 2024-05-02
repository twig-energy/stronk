#include <gtest/gtest.h>
#include <stronk/can_sqrt.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_double_sqrt_test_type : stronk<a_double_sqrt_test_type, double, can_sqrt, can_equate_with_is_close>
{
    using stronk::stronk;
};

TEST(can_sqrt, can_sqrt_stronk_double)
{
    constexpr auto a = a_double_sqrt_test_type {16};

    EXPECT_EQ(a.sqrt(), a_double_sqrt_test_type {4});
    EXPECT_EQ(sqrt(a), a_double_sqrt_test_type {4});
}

}  // namespace twig
