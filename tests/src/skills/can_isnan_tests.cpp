

#include <limits>

#include "stronk/skills/can_isnan.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct an_isnan_type : stronk<an_isnan_type, float, can_isnan>
{
    using stronk::stronk;
};

TEST_SUITE("can_isnan")
{
    TEST_CASE("isnan_works")
    {
        CHECK_FALSE(twig::isnan(an_isnan_type {0.F}));
        CHECK_FALSE(twig::isnan(an_isnan_type {1.F}));
        CHECK_FALSE(twig::isnan(an_isnan_type {-1.1F}));
        CHECK(twig::isnan(an_isnan_type {std::numeric_limits<float>::quiet_NaN()}));
        CHECK(twig::isnan(an_isnan_type {-std::numeric_limits<float>::quiet_NaN()}));
        CHECK(twig::isnan(an_isnan_type {std::numeric_limits<float>::signaling_NaN()}));
        CHECK(twig::isnan(an_isnan_type {-std::numeric_limits<float>::signaling_NaN()}));
    }

    TEST_CASE("quiet_NaN_isnan")
    {
        CHECK(twig::isnan(an_isnan_type::quiet_NaN()));
    }

    TEST_CASE("signaling_NaN_isnan")
    {
        CHECK(twig::isnan(an_isnan_type::signaling_NaN()));
    }
}

}  // namespace twig
