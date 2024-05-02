#include <gtest/gtest.h>
#include <stronk/can_decrement.h>
#include <stronk/stronk.h>

namespace twig
{

struct an_int_decrement_test_type : stronk<an_int_decrement_test_type, int, can_decrement, can_equate>
{
    using stronk::stronk;
};

TEST(can_decrement, can_decrement_stronk_integer)
{
    auto a = an_int_decrement_test_type {1};

    EXPECT_EQ(--a, an_int_decrement_test_type {0});
    EXPECT_EQ(a--, an_int_decrement_test_type {0});
    EXPECT_EQ(a, an_int_decrement_test_type {-1});
}

}  // namespace twig
