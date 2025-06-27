#include <fmt/format.h>
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ >= 12)

#    include <doctest/doctest.h>
#    include <fmt/core.h>

#    include "stronk/extensions/fmt.hpp"
#    include "stronk/stronk.hpp"

namespace twig
{

struct a_formattable_type : stronk<a_formattable_type, int, can_fmt_format_builder<"a_formattable_type({})">::skill>
{
    using stronk::stronk;
};

static_assert(a_formattable_type::fmt_string == "a_formattable_type({})");
static_assert(can_special_fmt_format_like<a_formattable_type>);

TEST_SUITE("can_fmt_format_builder")
{
    TEST_CASE("format_string_is_correctly_applied_via_can_fmt_format_builder")
    {
        CHECK_EQ(fmt::format("{}", a_formattable_type {5}), "a_formattable_type(5)");
        CHECK_EQ(fmt::format("{}", a_formattable_type {-12}), "a_formattable_type(-12)");
    }

    struct a_float_formattable_type : stronk<a_float_formattable_type, float, can_fmt_format_builder<"{:.4f}">::skill>
    {
        using stronk::stronk;
    };

    TEST_CASE("type_format_string_can_have_format_specifiers_applied")
    {
        CHECK_EQ(fmt::format("{}", a_float_formattable_type {42.0F}), "42.0000");
        CHECK_EQ(fmt::format("{:*^30}", a_float_formattable_type {42.0F}), "***********42.0000************");
    }
}

struct a_default_formattable_type : stronk<a_default_formattable_type, int, can_fmt_format>
{
    using stronk::stronk;
};

TEST_SUITE("can_fmt_format")
{
    TEST_CASE("format_string_is_correctly_applied_via_can_fmt_format")
    {
        CHECK_EQ(fmt::format("{}", a_default_formattable_type {42}), "42");
        CHECK_EQ(fmt::format("{}", a_default_formattable_type {-1}), "-1");
    }

    struct a_default_float_formattable_type : stronk<a_default_float_formattable_type, float, can_fmt_format>
    {
        using stronk::stronk;
    };

    TEST_CASE("format_string_can_have_format_specifiers_applied_if_underlying_type_supports_it")
    {
        CHECK_EQ(fmt::format("{:.3f}", a_default_float_formattable_type {42.0F}), "42.000");
        CHECK_EQ(fmt::format("{:*^30}", a_default_formattable_type {1}), "**************1***************");
    }
}

}  // namespace twig

#endif
