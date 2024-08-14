#include <format>

#include <gtest/gtest.h>
#include <stronk/can_format.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_default_formattable_type : stronk<a_default_formattable_type, int>
{
    using stronk::stronk;
};

TEST(can_format, format_string_is_correctly_applied_via_can_format)  // NOLINT
{
    EXPECT_EQ(std::format("{}", a_default_formattable_type {42}), "42");
    EXPECT_EQ(std::format("{}", a_default_formattable_type {-1}), "-1");
}

struct a_default_float_formattable_type : stronk<a_default_float_formattable_type, float>
{
    using stronk::stronk;
};

TEST(can_format, format_string_can_have_format_specifiers_applied_if_underlying_type_supports_it)  // NOLINT
{
    EXPECT_EQ(std::format("{:.3f}", a_default_float_formattable_type {42.0F}), "42.000");
    EXPECT_EQ(std::format("{:*^30}", a_default_formattable_type {1}), "**************1***************");
}

}  // namespace twig
