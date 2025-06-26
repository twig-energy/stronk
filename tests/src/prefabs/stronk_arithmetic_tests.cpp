

#include "stronk/prefabs/stronk_arithmetic.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_arithmetic_type : stronk_arithmetic<an_arithmetic_type, int>
{
    using stronk::stronk;
};

TEST_SUITE("stronk_arithmetic")
{
    TEST_CASE("can_perform_typical_operations")
    {
        auto stronk_number = an_arithmetic_type {1};

        CHECK_EQ(stronk_number, an_arithmetic_type {1});
        CHECK_NE(stronk_number, an_arithmetic_type {2});
        CHECK_EQ(stronk_number + an_arithmetic_type {2}, an_arithmetic_type {3});
        CHECK_EQ(stronk_number - an_arithmetic_type {2}, an_arithmetic_type {-1});
        CHECK_EQ(-stronk_number, an_arithmetic_type {-1});

        CHECK_LT(stronk_number, an_arithmetic_type {2});
        CHECK_LE(stronk_number, an_arithmetic_type {1});
        CHECK_GE(stronk_number, an_arithmetic_type {1});
        CHECK_GT(stronk_number, an_arithmetic_type {0});
    }
}

}  // namespace twig
