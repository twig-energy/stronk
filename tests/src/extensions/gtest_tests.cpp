#include <sstream>
#include <vector>

#include "stronk/extensions/gtest.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_gtest_print_type : stronk<a_can_gtest_print_type, int>
{
    using stronk::stronk;
};

TEST_SUITE("can_gtest_print")
{
    TEST_CASE("streaming to ostream prints the value")
    {
        auto val = a_can_gtest_print_type {5};
        auto sstream = std::stringstream();
        PrintTo(val, &sstream);  // this function is what gtest uses to print values

        CHECK_EQ(sstream.str(), "5");
    }

    struct a_cannot_gtest_print_type : stronk<a_can_gtest_print_type, std::vector<int>>
    {
        using stronk::stronk;
    };
    static_assert(is_stronk_and_can_ostream<a_can_gtest_print_type>);
    static_assert(!is_stronk_and_can_ostream<a_cannot_gtest_print_type>);
}

}  // namespace twig
