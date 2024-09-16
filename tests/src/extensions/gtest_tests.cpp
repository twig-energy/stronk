#include <sstream>

#include <gtest/gtest.h>
#include <stronk/extensions/gtest.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_can_gtest_print_type : stronk<a_can_gtest_print_type, int>
{
    using stronk::stronk;
};

TEST(can_gtest_print, streaming_to_ostream_prints_the_value)  // NOLINT
{
    auto val = a_can_gtest_print_type {5};
    auto sstream = std::stringstream();
    PrintTo(val, &sstream);

    EXPECT_EQ(sstream.str(), "5");
}

struct a_cannot_gtest_print_type : stronk<a_can_gtest_print_type, std::vector<int>>
{
    using stronk::stronk;
};
static_assert(is_stronk_and_can_ostream<a_can_gtest_print_type>);
static_assert(!is_stronk_and_can_ostream<a_cannot_gtest_print_type>);

}  // namespace twig
