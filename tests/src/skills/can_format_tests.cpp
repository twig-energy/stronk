#if __has_include(<format>)
#    include <format>

#    include "stronk/skills/can_format.hpp"

#    include <doctest/doctest.h>

#    include "stronk/stronk.hpp"

namespace twig
{

struct a_default_formattable_type : stronk<a_default_formattable_type, int>
{
    using stronk::stronk;
};

TEST_SUITE("can_format")
{
    TEST_CASE("format_string_is_correctly_applied_via_can_format")
    {
        CHECK_EQ(std::format("{}", a_default_formattable_type {42}), "42");
        CHECK_EQ(std::format("{}", a_default_formattable_type {-1}), "-1");
    }

    struct a_default_float_formattable_type : stronk<a_default_float_formattable_type, float>
    {
        using stronk::stronk;
    };

    TEST_CASE("format_string_can_have_format_specifiers_applied_if_underlying_type_supports_it")
    {
        CHECK_EQ(std::format("{:.3f}", a_default_float_formattable_type {42.0F}), "42.000");
        CHECK_EQ(std::format("{:*^30}", a_default_formattable_type {1}), "**************1***************");
    }
}

}  // namespace twig

#endif
