

#include "stronk/prefabs/stronk_string.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_string_type : stronk_string<a_string_type>
{
    using stronk::stronk;
};

TEST_SUITE("stronk_string")
{
    TEST_CASE("can_be_used_as_strings")
    {
        auto stronk_string = a_string_type {"hello"};
        CHECK_EQ(stronk_string.size(), 5);
        CHECK_EQ(stronk_string, a_string_type {"hello"});
        CHECK_NE(stronk_string, a_string_type {"HELLO"});
    }
}
}  // namespace twig
