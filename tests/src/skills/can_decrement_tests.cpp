#include "stronk/skills/can_decrement.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_int_decrement_test_type : stronk<an_int_decrement_test_type, int, can_decrement, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_decrement")
{
    TEST_CASE("can_decrement_stronk_integer")
    {
        auto a = an_int_decrement_test_type {1};

        CHECK_EQ(--a, an_int_decrement_test_type {0});
        CHECK_EQ(a--, an_int_decrement_test_type {0});
        CHECK_EQ(a, an_int_decrement_test_type {-1});
    }
}

}  // namespace twig
