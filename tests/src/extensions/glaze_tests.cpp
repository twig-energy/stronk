
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

struct an_int_can_glaze_de_and_serialize_wrapper
{
    an_int_can_glaze_de_and_serialize number;

    auto operator==(const an_int_can_glaze_de_and_serialize_wrapper& other) const -> bool = default;

    struct glaze
    {
        constexpr static auto value = glz::object(&an_int_can_glaze_de_and_serialize_wrapper::number);
    };
};

TEST(can_glaze_de_and_serialize, when_serializing_stronk_integer_then_can_deserialize_to_same_value)
{
    const auto val = an_int_can_glaze_de_and_serialize_wrapper {.number = an_int_can_glaze_de_and_serialize {42}};

    auto json_str = glz::write_json(val);
    EXPECT_EQ(R"({"number":42})", json_str);
    EXPECT_EQ(val, glz::read_json<an_int_can_glaze_de_and_serialize_wrapper>(json_str).value());
}

struct a_string_can_glaze_de_and_serialize
    : stronk<a_string_can_glaze_de_and_serialize, std::string, can_glaze_de_and_serialize, can_equate>
{
    using stronk::stronk;
};

struct a_string_can_glaze_de_and_serialize_wrapper
{
    a_string_can_glaze_de_and_serialize str;

    auto operator==(const a_string_can_glaze_de_and_serialize_wrapper& other) const -> bool = default;

    struct glaze
    {
        constexpr static auto value = glz::object(&a_string_can_glaze_de_and_serialize_wrapper::str);
    };
};

TEST(can_glaze_de_and_serialize, when_serializing_stronk_string_then_can_deserialize_to_same_value)
{
    const auto val = a_string_can_glaze_de_and_serialize_wrapper {.str = a_string_can_glaze_de_and_serialize {"hello"}};
    auto json_str = glz::write_json(val);
    EXPECT_EQ(R"({"str":"hello"})", json_str);
    EXPECT_EQ(val, glz::read_json<a_string_can_glaze_de_and_serialize_wrapper>(json_str).value());
}

struct a_class_with_two_can_glaze_members
{
    an_int_can_glaze_de_and_serialize val1;
    a_string_can_glaze_de_and_serialize val2;

    auto operator==(const a_class_with_two_can_glaze_members& other) const -> bool = default;

    struct glaze
    {
        constexpr static auto value =
            glz::object(&a_class_with_two_can_glaze_members::val1, &a_class_with_two_can_glaze_members::val2);
    };
};

TEST(can_glaze_de_and_serialize, when_serializing_a_type_with_multiple_members_it_roundtrips)
{
    const auto val = a_class_with_two_can_glaze_members {
        .val1 = an_int_can_glaze_de_and_serialize {24},
        .val2 = a_string_can_glaze_de_and_serialize {"world"},
    };
    auto json_str = glz::write_json(val);
    EXPECT_EQ(R"({"val1":24,"val2":"world"})", json_str);
    EXPECT_EQ(val, glz::read_json<a_class_with_two_can_glaze_members>(json_str).value());
}

}  // namespace twig
