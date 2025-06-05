

#include "stronk/prefabs/stronk_string.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_string_type : stronk_string<a_string_type>
{
    using stronk::stronk;
};

static_assert(a_string_type {"hello"}.size() == 5);

TEST(a_string_type, can_be_used_as_strings)
{
    auto stronk_string = a_string_type {"hello"};
    EXPECT_EQ(stronk_string.size(), 5);
    EXPECT_EQ(stronk_string, a_string_type {"hello"});
    EXPECT_NE(stronk_string, a_string_type {"HELLO"});
}

}  // namespace twig
