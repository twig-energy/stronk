#include <gtest/gtest.h>
#include <stronk/extensions/absl.h>
#include <stronk/extensions/fmt.h>
#include <stronk/extensions/gtest.h>
#include <stronk/extensions/nlohmann_json.h>
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

struct a_default_formattable_type : stronk<a_default_formattable_type, int, can_fmt_format>
{
    using stronk::stronk;
};

TEST(can_fmt_format, format_string_is_correctly_applied_via_can_fmt_format)  // NOLINT
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

struct a_can_gtest_print_type : stronk<a_can_gtest_print_type, int, can_gtest_print>
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

struct a_can_nlohmann_json : stronk<a_can_nlohmann_json, int, can_nlohmann_json, can_equate>
{
    using stronk::stronk;
};

TEST(can_nlohmann_json, when_serializing_stronk_integer_then_can_deserialize_to_same_value)
{
    const auto val = a_can_nlohmann_json {42};
    auto json_val = nlohmann::json {};
    json_val["number"] = val;
    EXPECT_EQ(R"({"number":42})", json_val.dump());
    EXPECT_EQ(val, json_val.at("number").get<a_can_nlohmann_json>());
}

struct a_string_can_nlohmann_json : stronk<a_string_can_nlohmann_json, std::string, can_nlohmann_json, can_equate>
{
    using stronk::stronk;
};

TEST(can_nlohmann_json, when_serializing_stronk_string_then_can_deserialize_to_same_value)
{
    const auto val = a_string_can_nlohmann_json {"hello"};
    auto json_val = nlohmann::json {};
    json_val["string"] = val;
    EXPECT_EQ(R"({"string":"hello"})", json_val.dump());
    EXPECT_EQ(val, json_val.at("string").get<a_string_can_nlohmann_json>());
}

}  // namespace twig
