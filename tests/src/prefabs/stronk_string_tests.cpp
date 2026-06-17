

#include <type_traits>

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
    TEST_CASE("can be used as strings")
    {
        auto stronk_string = a_string_type {"hello"};
        CHECK_EQ(stronk_string.size(), 5);
        CHECK_EQ(stronk_string, a_string_type {"hello"});
        CHECK_NE(stronk_string, a_string_type {"HELLO"});
    }

    TEST_CASE("can be converted to string_view and back")
    {
        auto stronk_string = a_string_type {"hello"};
        auto func = [](a_string_type::view_t view) -> void { CHECK_EQ(view, a_string_type::view_t {"hello"}); };
        func(stronk_string);

        auto and_back = static_cast<a_string_type>(static_cast<a_string_type::view_t>(stronk_string));
        CHECK_EQ(and_back, a_string_type {"hello"});
    }
}
}  // namespace twig
