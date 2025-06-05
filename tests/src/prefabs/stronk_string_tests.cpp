

#include "stronk/prefabs/stronk_string.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_string_type : stronk_string<a_string_type>
{
    using stronk::stronk;
};

TEST(stronk_string, can_be_used_as_strings)
{
    auto stronk_string = a_string_type {"hello"};
    EXPECT_EQ(stronk_string.size(), 5);
    EXPECT_EQ(stronk_string, a_string_type {"hello"});
    EXPECT_NE(stronk_string, a_string_type {"HELLO"});
}

}  // namespace twig
