#include <gtest/gtest.h>
#include <stronk/extensions/absl.h>
#include <stronk/extensions/fmt.h>
#include <stronk/extensions/gtest.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_formattable_type : stronk<a_formattable_type, int, can_fmt_format_builder<"a_formattable_type({})">::skill>
{
    using stronk::stronk;
};

TEST(can_fmt_format_builder, format_string_is_correctly_applied_via_fmt_format)  // NOLINT
{
    EXPECT_EQ(fmt::format("{}", a_formattable_type {5}), "a_formattable_type(5)");
    EXPECT_EQ(fmt::format("{}", a_formattable_type {-12}), "a_formattable_type(-12)");
}

struct a_default_formattable_type : stronk<a_default_formattable_type, int, can_fmt_format>
{
    using stronk::stronk;
};

TEST(can_fmt_format, format_string_is_correctly_applied_via_fmt_format)  // NOLINT
{
    EXPECT_EQ(fmt::format("{}", a_default_formattable_type {42}), "42");
    EXPECT_EQ(fmt::format("{}", a_default_formattable_type {-1}), "-1");
}

struct a_can_absl_hash_type : stronk<a_can_absl_hash_type, int, can_absl_hash>
{
    using stronk::stronk;
};

TEST(can_absl_hash, can_absl_hash_adds_the_absl_hash_function)  // NOLINT
{
    for (auto i = -10; i < 10; i++) {
        EXPECT_EQ(absl::HashOf(a_can_absl_hash_type(i)), absl::HashOf(i));
    }
}

}  // namespace twig
