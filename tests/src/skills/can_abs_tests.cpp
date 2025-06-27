

#include <cstdlib>

#include "stronk/skills/can_abs.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_abs_min_max_type : stronk<an_abs_min_max_type, int, can_order, can_equate, can_abs>
{
    using stronk::stronk;
};

TEST_SUITE("can_abs")
{
    TEST_CASE("abs_is_applied_correctly")
    {
        CHECK_EQ(twig::abs(an_abs_min_max_type {10}), an_abs_min_max_type {10});
        CHECK_EQ(twig::abs(an_abs_min_max_type {-10}), an_abs_min_max_type {10});

        CHECK_EQ(an_abs_min_max_type {10}.abs(), an_abs_min_max_type {10});
        CHECK_EQ(an_abs_min_max_type {-10}.abs(), an_abs_min_max_type {10});
    }

    TEST_CASE("abs_behaves_similar_to_integer_abs")
    {
        for (auto i = -16; i < 16; i++) {
            CHECK_EQ(an_abs_min_max_type {std::abs(i)}, twig::abs(an_abs_min_max_type {i}));
        }
    }
}

}  // namespace twig
