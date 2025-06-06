

#include "stronk/prefabs/stronk_arithmetic.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_arithmetic_type : stronk_arithmetic<an_arithmetic_type, int>
{
    using stronk::stronk;
};

TEST(stronk_arithmetic, can_perform_typical_operations)
{
    auto stronk_number = an_arithmetic_type {1};

    EXPECT_EQ(stronk_number, an_arithmetic_type {1});
    EXPECT_NE(stronk_number, an_arithmetic_type {2});
    EXPECT_EQ(stronk_number + an_arithmetic_type {2}, an_arithmetic_type {3});
    EXPECT_EQ(stronk_number - an_arithmetic_type {2}, an_arithmetic_type {-1});
    EXPECT_EQ(-stronk_number, an_arithmetic_type {-1});

    EXPECT_LT(stronk_number, an_arithmetic_type {2});
    EXPECT_LE(stronk_number, an_arithmetic_type {1});
    EXPECT_GE(stronk_number, an_arithmetic_type {1});
    EXPECT_GT(stronk_number, an_arithmetic_type {0});
}

}  // namespace twig
