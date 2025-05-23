

#include "stronk/skills/can_abs.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_abs_min_max_type : stronk<an_abs_min_max_type, int, can_order, can_equate, can_abs>
{
    using stronk::stronk;
};

TEST(can_abs, abs_is_applied_correctly)
{
    EXPECT_EQ(twig::abs(an_abs_min_max_type {10}), an_abs_min_max_type {10});
    EXPECT_EQ(twig::abs(an_abs_min_max_type {-10}), an_abs_min_max_type {10});

    EXPECT_EQ(an_abs_min_max_type {10}.abs(), an_abs_min_max_type {10});
    EXPECT_EQ(an_abs_min_max_type {-10}.abs(), an_abs_min_max_type {10});
}

TEST(can_abs, abs_behaves_similar_to_integer_abs)
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(an_abs_min_max_type {std::abs(i)}, twig::abs(an_abs_min_max_type {i}));
    }
}

}  // namespace twig
