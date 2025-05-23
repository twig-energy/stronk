

#include "stronk/skills/can_isnan.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_isnan_type : stronk<an_isnan_type, float, can_isnan>
{
    using stronk::stronk;
};

TEST(can_isnan, isnan_works)
{
    EXPECT_FALSE(twig::isnan(an_isnan_type {0.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {1.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {-1.1F}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::signaling_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::signaling_NaN()}));
}

TEST(can_isnan, quiet_NaN_isnan)
{
    EXPECT_TRUE(twig::isnan(an_isnan_type::quiet_NaN()));
}

TEST(can_isnan, signaling_NaN_isnan)
{
    EXPECT_TRUE(twig::isnan(an_isnan_type::signaling_NaN()));
}

}  // namespace twig
