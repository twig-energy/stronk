#pragma once
#include <nlohmann/json.hpp>
#include <stronk/stronk.h>

namespace twig
{

template<typename StronkT>
struct can_nlohmann_json
{
    constexpr static bool is_nlohmann_json_deserializable = true;
};

template<typename T>
concept can_nlohmann_json_like = stronk_like<T> && T::is_nlohmann_json_deserializable;

}  // namespace twig

namespace nlohmann
{

template<twig::can_nlohmann_json_like T>
struct adl_serializer<T>
{
    static void to_json(json& j, const T& value) { j = value.template unwrap<T>(); }

    static void from_json(const json& j, T& value) { j.get_to(value.template unwrap<T>()); }
};

}  // namespace nlohmann
