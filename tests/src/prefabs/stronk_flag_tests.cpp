
#include "stronk/prefabs/stronk_flag.hpp"

#include <gtest/gtest.h>

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

TEST(flag, flag_values_corrosponds_to_bool_values)
{
    EXPECT_FALSE(a_flag_type {false}.is_on());
    EXPECT_TRUE(a_flag_type {false}.is_off());
    EXPECT_TRUE(a_flag_type {true}.is_on());
    EXPECT_FALSE(a_flag_type {true}.is_off());

    EXPECT_FALSE(a_flag_type::off().is_on());
    EXPECT_TRUE(a_flag_type::off().is_off());

    EXPECT_TRUE(a_flag_type::on().is_on());
    EXPECT_FALSE(a_flag_type::on().is_off());
}

}  // namespace twig
