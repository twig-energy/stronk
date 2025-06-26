
#include "stronk/prefabs/stronk_flag.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_flag_type : stronk_flag<a_flag_type>
{
    using stronk::stronk;
};

static_assert(a_flag_type::on().is_on());
static_assert(!a_flag_type::on().is_off());
static_assert(!a_flag_type::off().is_on());
static_assert(a_flag_type::off().is_off());

TEST_SUITE("stronk_flag")
{
    TEST_CASE("flag_values_corrosponds_to_bool_values")
    {
        CHECK_FALSE(a_flag_type {false}.is_on());
        CHECK(a_flag_type {false}.is_off());
        CHECK(a_flag_type {true}.is_on());
        CHECK_FALSE(a_flag_type {true}.is_off());

        CHECK_FALSE(a_flag_type::off().is_on());
        CHECK(a_flag_type::off().is_off());

        CHECK(a_flag_type::on().is_on());
        CHECK_FALSE(a_flag_type::on().is_off());
    }
}

}  // namespace twig
