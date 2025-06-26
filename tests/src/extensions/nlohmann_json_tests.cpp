#include <string>

#include "stronk/extensions/nlohmann_json.hpp"

#include <doctest/doctest.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_nlohmann_json : stronk<a_can_nlohmann_json, int, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_nlohmann_json")
{
    TEST_CASE("when_serializing_stronk_integer_then_can_deserialize_to_same_value")
    {
        const auto val = a_can_nlohmann_json {42};
        auto json_val = nlohmann::json {};
        json_val["number"] = val;
        CHECK_EQ(R"({"number":42})", json_val.dump());
        CHECK_EQ(val, json_val.at("number").get<a_can_nlohmann_json>());
    }

    struct a_string_can_nlohmann_json : stronk<a_string_can_nlohmann_json, std::string, can_equate>
    {
        using stronk::stronk;
    };

    TEST_CASE("when_serializing_stronk_string_then_can_deserialize_to_same_value")
    {
        const auto val = a_string_can_nlohmann_json {"hello"};
        auto json_val = nlohmann::json {};
        json_val["string"] = val;
        CHECK_EQ(R"({"string":"hello"})", json_val.dump());
        CHECK_EQ(val, json_val.at("string").get<a_string_can_nlohmann_json>());
    }
}

}  // namespace twig
