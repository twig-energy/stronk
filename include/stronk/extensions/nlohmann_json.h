#pragma once
// IWYU pragma: always_keep

#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <stronk/stronk.h>

template<twig::stronk_like T>
struct nlohmann::adl_serializer<T>
{
    static void to_json(json& j, const T& value)
    {
        j = value.template unwrap<T>();
    }

    static void from_json(const json& j, T& value)
    {
        j.get_to(value.template unwrap<T>());
    }
};
