#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ >= 12)
#    include <array>
#    include <vector>

#    include "stronk/extensions/fmt.hpp"

#    include <doctest/doctest.h>
#    include <fmt/core.h>
#    include <fmt/format.h>
#    include <fmt/ranges.h>  //  IWYU pragma: keep

#    include "stronk/prefabs/stronk_string.hpp"
#    include "stronk/prefabs/stronk_vector.hpp"
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
    TEST_CASE("format string is correctly applied via can fmt format builder")
    {
        CHECK_EQ(fmt::format("{}", a_formattable_type {5}), "a_formattable_type(5)");
        CHECK_EQ(fmt::format("{}", a_formattable_type {-12}), "a_formattable_type(-12)");
    }

    struct a_float_formattable_type : stronk<a_float_formattable_type, float, can_fmt_format_builder<"{:.4f}">::skill>
    {
        using stronk::stronk;
    };

    TEST_CASE("type format string can have format specifiers applied")
    {
        CHECK_EQ(fmt::format("{}", a_float_formattable_type {42.0F}), "42.0000");
        CHECK_EQ(fmt::format("{:*^30}", a_float_formattable_type {42.0F}), "***********42.0000************");
    }
}

TEST_SUITE("can_fmt_format")
{
    struct a_default_formattable_type : stronk<a_default_formattable_type, int, can_fmt_format>
    {
        using stronk::stronk;
    };

    TEST_CASE("format string is correctly applied via can fmt format")
    {
        CHECK_EQ(fmt::format("{}", a_default_formattable_type {42}), "42");
        CHECK_EQ(fmt::format("{}", a_default_formattable_type {-1}), "-1");
    }

    struct a_default_float_formattable_type : stronk<a_default_float_formattable_type, float, can_fmt_format>
    {
        using stronk::stronk;
    };

    TEST_CASE("format string can have format specifiers applied if underlying_type supports it")
    {
        CHECK_EQ(fmt::format("{:.3f}", a_default_float_formattable_type {42.0F}), "42.000");
        CHECK_EQ(fmt::format("{:*^30}", a_default_formattable_type {1}), "**************1***************");
    }

    struct a_vector_stronk_type : stronk_vector<a_vector_stronk_type, int>
    {
        using stronk::stronk;
    };

    TEST_CASE("format on an vector like")
    {
        auto v = a_vector_stronk_type {1, 2, 3};
        CHECK_EQ(fmt::format("{}", v), "[1, 2, 3]");

        auto view = static_cast<a_vector_stronk_type::view_t>(v);
        CHECK_EQ(fmt::format("{}", view), "[1, 2, 3]");
    }

    struct an_array_stronk_type : stronk<an_array_stronk_type, std::array<int, 2>, can_iterate>
    {
        using stronk::stronk;
    };

    TEST_CASE("format on an array like")
    {
        auto v = an_array_stronk_type {std::array {1, 2}};
        CHECK_EQ(fmt::format("{}", v), "[1, 2]");
    }

    struct a_string_type : stronk_string<a_string_type>
    {
        using stronk::stronk;
    };

    TEST_CASE("format on an string should be printed normally")
    {
        auto v = a_string_type {"hello"};
        CHECK_EQ(fmt::format("{}", v), "hello");

        auto also_as_string_view = static_cast<a_string_type::view_t>(v);
        CHECK_EQ(fmt::format("{}", also_as_string_view), "hello");
    }

    struct a_string_type_with_iterators : stronk_string<a_string_type_with_iterators, can_const_iterate>
    {
        using stronk::stronk;
    };

    TEST_CASE("format on an string with iterators should be printed normally")
    {
        auto v = a_string_type_with_iterators {"hello"};
        CHECK_EQ(fmt::format("{}", v), "hello");

        auto view = static_cast<a_string_type_with_iterators::view_t>(v);
        CHECK_EQ(fmt::format("{}", view), "hello");
    }
}

}  // namespace twig

#endif
