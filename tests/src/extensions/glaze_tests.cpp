
#include <glaze/glaze.hpp>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <stronk/extensions/glaze.h>
#include <stronk/stronk.h>

namespace twig
{

struct an_int_can_glaze_de_and_serialize
    : stronk<an_int_can_glaze_de_and_serialize, int, can_glaze_de_and_serialize, can_equate>
{
    using stronk::stronk;
};

TEST(can_glaze_de_and_serialize, when_serializing_stronk_integer_then_can_deserialize_to_same_value)
{
    const auto val = an_int_can_glaze_de_and_serialize {42};

    auto json_str = glz::write_json(val);
    EXPECT_EQ(R"(42)", json_str);
    EXPECT_EQ(val, glz::read_json<an_int_can_glaze_de_and_serialize>(json_str));
}

struct a_string_can_glaze_de_and_serialize
    : stronk<a_string_can_glaze_de_and_serialize, std::string, can_glaze_de_and_serialize, can_equate>
{
    using stronk::stronk;
};

TEST(can_glaze_de_and_serialize, when_serializing_stronk_string_then_can_deserialize_to_same_value)
{
    const auto val = a_string_can_glaze_de_and_serialize {"hello"};
    auto json_str = glz::write_json(val);
    EXPECT_EQ(R"("hello")", json_str);
    EXPECT_EQ(val, glz::read_json<a_string_can_glaze_de_and_serialize>(json_str));
}

}  // namespace twig
