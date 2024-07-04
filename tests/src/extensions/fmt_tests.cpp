#include <fmt/core.h>
#include <gtest/gtest.h>
#include <stronk/extensions/fmt.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_formattable_type : stronk<a_formattable_type, int, can_fmt_format_builder<"a_formattable_type({})">::skill>
{
    using stronk::stronk;
};

TEST(can_fmt_format_builder, format_string_is_correctly_applied_via_can_fmt_format_builder)  // NOLINT
{
    EXPECT_EQ(fmt::format("{}", a_formattable_type {5}), "a_formattable_type(5)");
    EXPECT_EQ(fmt::format("{}", a_formattable_type {-12}), "a_formattable_type(-12)");
}

struct a_float_formattable_type : stronk<a_float_formattable_type, float, can_fmt_format_builder<"{:.4f}">::skill>
{
    using stronk::stronk;
};

TEST(can_fmt_format_builder, type_format_string_can_have_format_specifiers_applied)  // NOLINT
{
    EXPECT_EQ(fmt::format("{}", a_float_formattable_type {42.0F}), "42.0000");
    EXPECT_EQ(fmt::format("{:*^30}", a_float_formattable_type {42.0F}), "***********42.0000************");
}

struct a_default_formattable_type : stronk<a_default_formattable_type, int, can_fmt_format>
{
    using stronk::stronk;
};

TEST(can_fmt_format, format_string_is_correctly_applied_via_can_fmt_format)  // NOLINT
{
    EXPECT_EQ(fmt::format("{}", a_default_formattable_type {42}), "42");
    EXPECT_EQ(fmt::format("{}", a_default_formattable_type {-1}), "-1");
}

struct a_default_float_formattable_type : stronk<a_default_float_formattable_type, float, can_fmt_format>
{
    using stronk::stronk;
};

TEST(can_fmt_format, format_string_can_have_format_specifiers_applied_if_underlying_type_supports_it)  // NOLINT
{
    EXPECT_EQ(fmt::format("{:.3f}", a_default_float_formattable_type {42.0F}), "42.000");
    EXPECT_EQ(fmt::format("{:*^30}", a_default_formattable_type {1}), "**************1***************");
}

}  // namespace twig
