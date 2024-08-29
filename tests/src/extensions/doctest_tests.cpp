#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>
#include <gtest/gtest.h>
#include <stronk/extensions/doctest.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_type_to_doctest_print : stronk<a_type_to_doctest_print, int>
{
    using stronk::stronk;
};

struct a_recursive_type_to_doctest_print : stronk<a_recursive_type_to_doctest_print, a_type_to_doctest_print>
{
    using stronk::stronk;
};

TEST(can_doctest_extend, when_given_a_stronk_type_doctest_can_convert_the_underlying_string)  // NOLINT
{
    EXPECT_EQ(doctest::StringMaker<a_type_to_doctest_print>::convert(a_type_to_doctest_print {5}), "5");
    EXPECT_EQ(doctest::StringMaker<a_recursive_type_to_doctest_print>::convert(
                  a_recursive_type_to_doctest_print {a_type_to_doctest_print {42}}),
              "42");
}

}  // namespace twig
